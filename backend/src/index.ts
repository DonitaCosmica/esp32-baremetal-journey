import { createServer, IncomingMessage, ServerResponse } from 'node:http'
import { connect } from 'mqtt'

const PORT = Number(process.env.PORT ?? 3000)
const MQTT_URL = process.env.MQTT_URL ?? 'mqtt://192.168.100.11:1883'

const TOPIC_LED = 'telemetry/s3/led'
const TOPIC_STATUS = 'telemetry/s3/status'

interface LedState {
  led: 0 | 1,
  ts: number,
  receivedAt: number
}

let latestLed: LedState | null = null
let deviceOnline = false

const mqttClient = connect(MQTT_URL, { reconnectPeriod: 2000 })

mqttClient.on('connect', () => {
  console.log('Conectado al broker MQTT')
  mqttClient.subscribe([TOPIC_LED, TOPIC_STATUS], (err) => {
    if(err) console.error('Error al suscribirse: ', err.message)
  })
})

mqttClient.on('error', (err) => console.error('MQTT error: ', err.message))
mqttClient.on('offline', () => console.warn('MQTT sin conexion al broker'))
mqttClient.on('message', (topic: string, payload: Buffer) => {
  const text = payload.toString().trim()
  if(text === '') return

  switch(topic) {
    case TOPIC_STATUS: {
      deviceOnline = text === 'online'
      console.log('Estado del dispositivo: ', text)
      return
    }

    case TOPIC_LED: {
      try {
        const data: LedState = JSON.parse(text)
        if((data.led !== 0 && data.led !== 1) || typeof data.ts !== 'number') {
          throw new Error('formato insesperado')
        }

        latestLed = { led: data.led, ts: data.ts, receivedAt: Date.now() }
      } catch (error) {
        console.error(`Mensaje invalido en ${ topic }: ${ text }`)
      }
    }
  }
})

const CORS = {
  'Access-Control-Allow-Origin': '*',
  'Access-Control-Allow-Methods': 'GET, OPTIONS',
  'Access-Control-Max-Age': '86400',
}

const send = (res: ServerResponse, code: number, body?: unknown) => {
  res.writeHead(code, { ...CORS, 'Content-Type': 'application/json' })
  res.end(body === undefined ? undefined : JSON.stringify(body))
}

const requestListener = (req: IncomingMessage, res: ServerResponse) => {
  if (req.method === 'OPTIONS') return send(res, 204)
  if (req.method !== 'GET') return send(res, 405, { error: 'Method not allowed' })

  const path = (req.url ?? '').split('?')[0]

  if(path === '/api/s3/led') {
    return send(res, 200, {
      led: latestLed?.led ?? null,
      ts: latestLed?.ts ?? null,
      online: deviceOnline,
      receivedAt: latestLed?.receivedAt ?? null,
    })
  }

  send(res, 404, { error: 'Resource not found' })
}


const server = createServer(requestListener)
server.listen(PORT, () => {
  console.log(`Server is running on http://localhost:${ PORT }`)
})

const shutdown = () => {
  console.log('Cerrando...')
  mqttClient.end(true)
  server.close(() => process.exit(0))
}

process.on('SIGINT', shutdown)
process.on('SIGTERM', shutdown)