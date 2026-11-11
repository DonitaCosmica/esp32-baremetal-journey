import type { Device, DeviceRef, Telemetry } from './types'

type Listener = (device: Device) => void

const devices = new Map<string, Device>()
const listeners = new Set<Listener>()

const emit = (device: Device) => {
  for(const fn of listeners)
    fn(device)
}

const upsert = (ref: DeviceRef): Device => {
  const existing = devices.get(ref.key)
  if(existing) return existing

  const created: Device = { ...ref, online: false, telemetry: null, receivedAt: null }
  devices.set(ref.key, created)
  return created
}

const setAllOffline = (match: (device: Device) => boolean): void => {
  for(const device of devices.values()) {
    if(device.online && match(device)) {
      device.online = false
      emit(device)
    }
  }
}

export const listDevices = (): Device[] => [...devices.values()]
export const getDevice = (key: string): Device | undefined => devices.get(key)
export const markAllOffline = () => setAllOffline(() => true)
export const markViaOffline = (gatewayId: string) =>setAllOffline((device: Device) => device.telemetry?.via === gatewayId)

export const setTelemetry = (ref: DeviceRef, telemetry: Telemetry): void => {
  const device = upsert(ref)

  device.telemetry = telemetry;
  device.receivedAt = Date.now()
  emit(device)
}

export const setOnline = (ref: DeviceRef, online: boolean): void => {
  const device = upsert(ref)

  if(device.online === online) return
  device.online = online
  emit(device)
}

export const subscribe = (fn: Listener) => {
  listeners.add(fn)
  return () => listeners.delete(fn)
}