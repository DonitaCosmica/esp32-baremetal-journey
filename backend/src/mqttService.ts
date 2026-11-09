import { connect, type MqttClient } from 'mqtt'
import { config } from './config'
import { setLed, setOnline, setProximity } from './deviceStore'
import type { Sample } from './types'

const parseSample = (text: string, key: 'led' | 'detected'): Sample | null => {
  try {
    const data: unknown = JSON.parse(text)
    if (typeof data !== 'object' || data === null) return null

    const record = data as Record<string, unknown>
    const value = record[key]
    const ts = record.ts
    if ((value !== 0 && value !== 1) || typeof ts !== 'number') return null

    return { value, ts }
  } catch {
    return null
  }
}

export const startMqtt = (): MqttClient => {
  const client = connect(config.mqttUrl, { reconnectPeriod: 2000 })

  client.on('connect', () => {
    console.log('Conectado al broker MQTT')
    client.subscribe(Object.values(config.topics), (err) => {
      if (err) console.error('Error al suscribirse:', err.message)
    })
  })

  client.on('error', (err) => console.error('MQTT error:', err.message))
  client.on('offline', () => console.warn('MQTT sin conexión al broker'))
  client.on('close', () => setOnline(false))

  client.on('message', (topic, payload) => {
    const text = payload.toString().trim()
    if (text === '') return

    switch (topic) {
      case config.topics.status:
        console.log('Estado del dispositivo:', text)
        setOnline(text === 'online')
        break

      case config.topics.led: {
        const s = parseSample(text, 'led')
        if (s) setLed(s)
        else console.error(`Mensaje inválido en ${topic}: ${text}`)
        break
      }

      case config.topics.proximity: {
        const s = parseSample(text, 'detected')
        if (s) setProximity(s)
        else console.error(`Mensaje inválido en ${topic}: ${text}`)
        break
      }
    }
  })

  return client
}