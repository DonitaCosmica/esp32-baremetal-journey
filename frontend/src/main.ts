import { STREAM_URL } from './config.js'
import { watchDevices } from './stream.js'
import { renderDevice } from './render.js'

const root = document.getElementById('devices')
const conn = document.getElementById('conn')
if(!root || !conn) throw new Error('Faltan elementos en el HTML')

watchDevices(
  STREAM_URL,
  (list) => list.forEach((device) => renderDevice(root, device)),
  (device) => renderDevice(root, device),
  (ok) => { conn.textContent = ok ? 'Conectado' : 'Sin conexión con el servidor' },
)