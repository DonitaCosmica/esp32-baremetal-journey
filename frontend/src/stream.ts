import type { DeviceState } from './types.js'

export const watchDevice = (
  url: string,
  onState: (state: DeviceState) => void,
  onError: () => void,
): (() => void) => {
  const source = new EventSource(url)

  source.onmessage = (event: MessageEvent<string>) => {
    try {
      onState(JSON.parse(event.data) as DeviceState)
    } catch {
      console.error('Evento inválido:', event.data)
    }
  }

  source.onerror = onError

  return () => source.close()
}