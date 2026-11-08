import type { IncomingMessage, ServerResponse } from 'node:http'
import { config } from './config'
import { getSnapshot, subscribe } from './deviceStore'
import type { DeviceState } from './types'

const clients = new Set<ServerResponse>()

const write = (res: ServerResponse, state: DeviceState) =>
  res.write(`data: ${JSON.stringify(state)}\n\n`)

export const startSse = () => {
  subscribe((state) => {
    for(const client of clients) write(client, state)
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
  write(res, getSnapshot())

  clients.add(res)
  req.on('close', () => clients.delete(res))
}

export const closeSseClients = () => {
  for(const client of clients) client.end()
  clients.clear()
}