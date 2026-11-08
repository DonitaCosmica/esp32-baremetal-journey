import { connect, type MqttClient } from 'mqtt'
import { config } from './config'
import { setLed, setOnline } from './deviceStore'
import type { LedMessage } from './types'

const parseLed = (text: string): LedMessage | null => {
  try {
    const data: unknown = JSON.parse(text)
    if(typeof data !== 'object' || data === null) return null

    const { led, ts } = data as Record<string, unknown>
    if((led !== 0 && led !== 1) || typeof ts !== 'number') return null

    return { led, ts }
  } catch {
    return null
  }
}

export const startMqtt = (): MqttClient => {
  const client = connect(config.mqttUrl, { reconnectPeriod: 2000 })

  client.on('connect', () => {
    console.log('Conectado al broker MQTT')
    client.subscribe([config.topics.led, config.topics.status], (err) => {
      if(err) console.error('Error al suscribirse:', err.message)
    })
  })

  client.on('error', (err) => console.error('MQTT error:', err.message))
  client.on('offline', () => console.warn('MQTT sin conexión al broker'))
  client.on('close', () => setOnline(false))
  client.on('message', (topic, payload) => {
    const text = payload.toString().trim()
    if(text === '') return

    if(topic === config.topics.status) {
      console.log('Estado del dispositivo:', text)
      setOnline(text === 'online')
    } else if(topic === config.topics.led) {
      const msg = parseLed(text)
      if(msg) setLed(msg)
      else console.error(`Mensaje inválido en ${ topic }: ${ text }`)
    }
  })

  return client
}