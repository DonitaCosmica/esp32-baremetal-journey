import { config } from './config'
import { startMqtt } from './mqttService'
import { startSse, closeSseClients } from './sse'
import { createHttpServer } from './http'

startSse()
const mqttClient = startMqtt()
const server = createHttpServer()

server.listen(config.port, () => {
  console.log(`Server en http://localhost:${ config.port }`)
})

const shutdown = () => {
  console.log('Cerrando...')
  closeSseClients()
  mqttClient.end(true)
  server.close(() => process.exit(0))
}

process.on('SIGINT', shutdown)
process.on('SIGTERM', shutdown)