interface DeviceState {
  led: 0 | 1,
  ts: number | null,
  online: boolean,
  receivedAt: number | null
}

interface Device {
  id: string,
  url: string
}

const API = 'http://localhost:3000'

const DEVICES: Device[] = [
  { id: 'esp32-s3', url: `${ API }/api/s3/led/stream` }
]

const render = (element: HTMLElement, state: DeviceState): void => {
  const led = state.led === 1 ? 'ON' : state.led === 0 ? 'OFF' : '—'
  element.textContent = `LED: ${ led }` + (state.online ? '' : ' (dispositivo offline)')
}

const connect = ({ id, url }: Device): void => {
  const element = document.getElementById(id)
  if(!element) {
    console.error(`No existe el elemento #${ id }`)
    return
  }

  const source = new EventSource(url)
  source.onmessage = (event: MessageEvent<string>) => {
    try {
      render(element, JSON.parse(event.data) as DeviceState)
    } catch {
      console.error('Evento invalido: ', event.data)
    }
  }

  source.onerror = () => {
    element.textContent = 'LED: sin conexion'
  }
}

DEVICES.forEach(connect)