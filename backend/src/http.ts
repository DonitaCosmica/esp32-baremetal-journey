import { createServer, type IncomingMessage, type ServerResponse } from 'node:http'
import { getSnapshot } from './deviceStore'
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

  const path = (req.url ?? '').split('?')[0]

  switch (path) {
    case '/api/s3/led':
      return send(res, 200, getSnapshot())
    case '/api/s3/led/stream':
      return addSseClient(req, res, CORS)
    default:
      return send(res, 404, { error: 'Resource not found' })
  }
}

export const createHttpServer = () => createServer(listener)