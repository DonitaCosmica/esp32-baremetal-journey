export interface DeviceState {
  led: 0 | 1 | null
  ts: number | null
  online: boolean
  receivedAt: number | null
}

export interface Device {
  id: string
  url: string
}