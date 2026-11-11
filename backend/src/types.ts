export type Value = number | string
export type Group = Record<string, string>

export interface Telemetry {
  ts: number,
  via?: string,
  metrics: Group,
  sensors_health: Group,
  diagnostics: Group
}

export interface DeviceRef { // region/zone/id
  key: string,
  region: string,
  zone: string,
  id: string
}

export interface Device extends DeviceRef {
  online: boolean,
  telemetry: Telemetry | null,
  receivedAt: number | null
}