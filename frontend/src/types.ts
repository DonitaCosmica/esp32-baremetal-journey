export type Value = number | string
export type Group = Record<string, Value>

export interface Telemetry {
  ts: number
  via?: string
  metrics: Group
  sensors_health: Group
  diagnostics: Group
}

export interface Device {
  key: string
  region: string
  zone: string
  id: string
  online: boolean
  telemetry: Telemetry | null
  receivedAt: number | null
}