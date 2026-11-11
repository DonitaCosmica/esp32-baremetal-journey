import type { Device } from './types.js'

const parse = <T>(event: Event): T => JSON.parse((event as MessageEvent<string>).data) as T

export const watchDevices = (
  url: string,
  onSnapshot: (devices: Device[]) => void,
  onDevice: (device: Device) => void,
  onConnection: (connected: boolean) => void,
): (() => void) => {
  const source = new EventSource(url)

  source.onopen = () => onConnection(true)
  source.onerror = () => onConnection(false)
  source.addEventListener('snapshot', (event) => onSnapshot(parse<Device[]>(event)))
  source.addEventListener('device', (event) => onDevice(parse<Device>(event)))

  return () => source.close()
}