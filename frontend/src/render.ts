import type { DeviceState, Reading } from './types.js'

const label = (r: Reading | null, on: string, off: string): string =>
  r === null ? '—' : r.value === 1 ? on : off

const offline = (s: DeviceState): string => (s.online ? '' : ' (dispositivo offline)')

export const renderLed = (el: HTMLElement, s: DeviceState): void => {
  el.textContent = `LED: ${label(s.led, 'ON', 'OFF')}${offline(s)}`
}

export const renderProximity = (el: HTMLElement, s: DeviceState): void => {
  el.textContent = `Sensor: ${label(s.proximity, 'objeto detectado', 'objeto no detectado')}${offline(s)}`
}

export const renderDisconnected = (el: HTMLElement, name: string): void => {
  el.textContent = `${name}: sin conexión`
}