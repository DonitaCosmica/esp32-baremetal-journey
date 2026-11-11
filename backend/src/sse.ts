import type { IncomingMessage, ServerResponse } from 'node:http'
import { config } from './config'
import { listDevices, subscribe } from './deviceStore'
import { Device } from './types'

const clients = new Set<ServerResponse>()

const send = (res: ServerResponse, event: string, data: unknown): Boolean =>
  res.write(`event: ${event}\ndata: ${JSON.stringify(data)}\n\n`)

export const startSse = () => {
  subscribe((device: Device) => {
    for(const client of clients) send(client, 'device', device)
  })

  setInterval(() => {
    for(const client of clients) client.write(': ping\n\n')
  }, config.sseKeepAliveMs).unref()
}

export const addSseClient = (
  req: IncomingMessage,
  res: ServerResponse,
  cors: Record<string, string>,
) => {
  res.writeHead(200, {
    ...cors,
    'Content-Type': 'text/event-stream',
    'Cache-Control': 'no-cache',
    Connection: 'keep-alive',
    'X-Accel-Buffering': 'no',
  })

  res.write('retry: 2000\n\n')
  send(res, 'snapshot', listDevices())

  clients.add(res)
  req.on('close', () => clients.delete(res))
}

export const closeSseClients = () => {
  for(const client of clients) client.end()
  clients.clear()
}