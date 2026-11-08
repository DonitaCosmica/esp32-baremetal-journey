import { DEVICES } from './config.js'
import { watchDevice } from './stream.js'
import { renderState, renderDisconnected } from './render.js'

for (const { id, url } of DEVICES) {
  const el = document.getElementById(id)
  if (!el) {
    console.error(`No existe el elemento #${id}`)
    continue
  }
  watchDevice(url, (s) => renderState(el, s), () => renderDisconnected(el))
}