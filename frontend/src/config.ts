import type { Device } from './types.js'

const API = 'http://localhost:3000'

export const DEVICES: Device[] = [
  { id: 'esp32-s3', url: `${API}/api/s3/led/stream` },
]