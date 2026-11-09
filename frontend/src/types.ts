export interface Reading {
  value: 0 | 1
  ts: number
  receivedAt: number
}

export interface DeviceState {
  online: boolean
  led: Reading | null
  proximity: Reading | null
}