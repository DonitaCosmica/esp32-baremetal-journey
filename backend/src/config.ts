export const config = {
  port: Number(process.env.PORT ?? 3000),
  mqttUrl: process.env.MQTT_URL ?? 'mqtt://192.168.100.11:1883',
  org: process.env.MQTT_ORG ?? 'hackathon',
  sseKeepAliveMs: 25_000,
} as const