export interface LedMessage {
  led: 0 | 1
  ts: number
}

export interface DeviceState {
  led: 0 | 1 | null
  ts: number | null
  online: boolean
  receivedAt: number | null
}