import type { DeviceState, LedMessage } from "./types";

type Listener = (state: DeviceState) => void

let reading: (LedMessage & { receivedAt: number }) | null = null
let online = false
const listeners = new Set<Listener>()

export const getSnapshot = (): DeviceState => ({
  led: reading?.led ?? null,
  ts: reading?.ts ?? null,
  online,
  receivedAt: reading?.receivedAt ?? null,
})

const emit = () => {
  const state = getSnapshot()
  for(const fn of listeners) fn(state)
}

export const setLed = (msg: LedMessage) => {
  reading = { ...msg, receivedAt: Date.now() }
  emit()
}

export const setOnline = (value: boolean) => {
  if(online === value) return
  online = value
  emit()
}

export const subscribe = (fn: Listener) => {
  listeners.add(fn)
  return () => listeners.delete(fn)
}