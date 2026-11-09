import type { DeviceState, Reading, Sample } from './types'

type Listener = (state: DeviceState) => void

let led: Reading | null = null
let proximity: Reading | null = null
let online = false
const listeners = new Set<Listener>()

export const getSnapshot = (): DeviceState => ({ online, led, proximity })

const emit = () => {
  const state = getSnapshot()
  for (const fn of listeners) fn(state)
}

export const setLed = (s: Sample) => {
  led = { ...s, receivedAt: Date.now() }
  emit()
}

export const setProximity = (s: Sample) => {
  proximity = { ...s, receivedAt: Date.now() }
  emit()
}

export const setOnline = (value: boolean) => {
  if (online === value) return
  online = value
  emit()
}

export const subscribe = (fn: Listener) => {
  listeners.add(fn)
  return () => listeners.delete(fn)
}