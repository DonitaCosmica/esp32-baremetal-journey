import { connect, type MqttClient } from 'mqtt'
import { config } from './config'
import { markViaOffline, markAllOffline, setOnline, setTelemetry } from './deviceStore'
import type { DeviceRef, Group, Telemetry } from './types'

const parseTopic = (topic: string): { ref: DeviceRef, type: string } | null => {
  const [org, region, zone, id, type, ...rest] = topic.split('/')
  if(org !== config.org || !region || !zone || !id || !type || rest.length) return null
  return { ref: { key: `${ region }/${ zone }/${ id }`, region, zone, id }, type }
}

const isGroup = (value: unknown): value is Group =>
  typeof value === 'object' && value !== null && !Array.isArray(value) &&
  Object.values(value).every((x) => typeof x === 'number' || typeof x === 'string')

const parseTelemetry = (text: string): Telemetry | null => {
  try {
    const device = JSON.parse(text) as Record<string, unknown>
    if(typeof device !== 'object' || typeof device === null || typeof device.ts !== 'number') return null

    const metrics = device.metrics ?? {}
    const sensors_health = device.sensors_health ?? {}
    const diagnostics = device.diagnostics ?? {}
    if(!isGroup(metrics) || !isGroup(sensors_health) || !isGroup(diagnostics)) return null

    return {
      ts: device.ts,
      via: typeof device.via === 'string' ? device.via : undefined,
      metrics,
      sensors_health,
      diagnostics
    }
  } catch {
    return null
  }
}

export const startMqtt = (): MqttClient => {
  const client = connect(config.mqttUrl, { reconnectPeriod: 2000 })

  client.on('connect', () => {
    console.log('Conectado al broker MQTT')
    
    const filters = [`${config.org}/+/+/+/telemetry`, `${config.org}/+/+/+/status`]
    client.subscribe(filters, (err) => {
      if(err) console.error('Error al suscribirse:', err.message)
    })
  })

  client.on('error', (err) => console.error('MQTT error:', err.message))
  client.on('offline', () => console.warn('MQTT sin conexión al broker'))
  client.on('close', () => markAllOffline())

  client.on('message', (topic, payload) => {
    const text = payload.toString().trim()
    const t = parseTopic(topic)
    if (text === '' || !t) return

    switch (t.type) {
      case 'status': {
        const online = text === 'online'
        setOnline(t.ref, online)
        if(!online) markViaOffline(t.ref.id)
      }

      case 'telemetry': {
        const telemetry = parseTelemetry(text)
        if(telemetry) setTelemetry(t.ref, telemetry)
        else console.error(`Telemetría inválida en ${topic}: ${text}`)
      }
    }
  })

  return client
}