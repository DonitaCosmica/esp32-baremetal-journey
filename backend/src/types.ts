export interface Sample {
  value: 0 | 1
  ts: number
}

export interface Reading extends Sample {
  receivedAt: number
}

export interface DeviceState {
  online: boolean
  led: Reading | null
  proximity: Reading | null
}