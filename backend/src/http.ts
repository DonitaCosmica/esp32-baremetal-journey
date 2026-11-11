import { createServer, type IncomingMessage, type ServerResponse } from 'node:http'
import { getDevice, listDevices } from './deviceStore'
import { addSseClient } from './sse'

const CORS = {
  'Access-Control-Allow-Origin': '*',
  'Access-Control-Allow-Methods': 'GET, OPTIONS',
  'Access-Control-Max-Age': '86400',
}

const send = (res: ServerResponse, code: number, body?: unknown) => {
  res.writeHead(code, { ...CORS, 'Content-Type': 'application/json' })
  res.end(body === undefined ? undefined : JSON.stringify(body))
}

const listener = (req: IncomingMessage, res: ServerResponse) => {
  if (req.method === 'OPTIONS') return send(res, 204)
  if (req.method !== 'GET') return send(res, 405, { error: 'Method not allowed' })

  const path = (req.url ?? '').split('?')[0] ?? ''

  if (path === '/api/devices') return send(res, 200, listDevices())
  if (path === '/api/stream') return addSseClient(req, res, CORS)

  const match = path.match(/^\/api\/devices\/([^/]+)\/([^/]+)\/([^/]+)$/)
  if(match) {
    const device = getDevice(`${match[1]}/${match[2]}/${match[3]}`)
    return device ? send(res, 200, device) : send(res, 404, { error: 'Device not found' })
  }

  send(res, 404, { error: 'Resource not found' })
}

export const createHttpServer = () => createServer(listener)