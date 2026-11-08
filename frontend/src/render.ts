import type { DeviceState } from './types.js'

export const renderState = (el: HTMLElement, state: DeviceState): void => {
  const led = state.led === 1 ? 'ON' : state.led === 0 ? 'OFF' : '—'
  el.textContent = `LED: ${led}` + (state.online ? '' : ' (dispositivo offline)')
}

export const renderDisconnected = (el: HTMLElement): void => {
  el.textContent = 'LED: sin conexión'
}