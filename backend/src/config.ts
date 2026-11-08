export const config = {
  port: Number(process.env.PORT ?? 3000),
  mqttUrl: process.env.MQTT_URL ?? 'mqtt://192.168.100.11:1883',
  topics: {
    led: 'telemetry/s3/led',
    status: 'telemetry/s3/status',
  },
  sseKeepAliveMs: 25_000,
} as const