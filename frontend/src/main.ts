import { STREAM_URL } from './config.js'
import { watchDevice } from './stream.js'
import { renderLed, renderProximity, renderDisconnected } from './render.js'

const ledEl = document.getElementById('s3-led')
const proxEl = document.getElementById('wroom-proximity')
if (!ledEl || !proxEl) throw new Error('Faltan elementos en el HTML')

watchDevice(
  STREAM_URL,
  (state) => {
    renderLed(ledEl, state)
    renderProximity(proxEl, state)
  },
  () => {
    renderDisconnected(ledEl, 'LED')
    renderDisconnected(proxEl, 'Sensor')
  },
)