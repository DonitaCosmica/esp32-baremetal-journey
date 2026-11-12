import type { Device, Group, Value } from './types.js'

const TITLES = {
  metrics: 'Mediciones',
  sensors_health: 'Salud de sensores',
  diagnostics: 'Diagnóstico',
} as const

const FORMAT: Record<string, (value: Value) => string> = {
  led: (value) => (value ? 'ON' : 'OFF'),
  object_detected: (value) => (value ? 'objeto detectado' : 'objeto no detectado'),
}

const cards = new Map<string, HTMLElement>()

const element = (tag: string, cls?: string, text?: string): HTMLElement => {
  const el = document.createElement(tag)
  
  if(cls) el.className = cls
  if(text !== undefined) el.textContent = text

  return el
}

const groupBox = (title: string, group: Group): HTMLElement => {
  const box = element('div', 'group')
  box.append(element('h4', undefined, title))

  for(const [key, value] of Object.entries(group)) {
    const row = element('div', 'row')
    row.append(element('span', 'k', key), element('span', 'v', FORMAT[key]?.(value) ?? String(value)))
    box.append(row)
  }

  return box
}

const getCard = (root: HTMLElement, key: string): HTMLElement => {
  const existing = cards.get(key)
  if(existing) return existing

  const created = element('section', 'card')
  cards.set(key, created)
  root.append(created)
  
  return created
}

export const renderDevice = (root: HTMLElement, device: Device): void => {
  const card = getCard(root, device.key)

  const head = element('div', 'head')
  head.append(
    element('strong', undefined, device.id),
    element('span', device.online ? 'badge on' : 'badge off', device.online ? 'online' : 'offline'),
  )

  card.replaceChildren(head, element('small', undefined, `${device.region} / ${device.zone}`))

  if(device.telemetry) {
    for(const [k, title] of Object.entries(TITLES)) {
      const group = device.telemetry[k as keyof typeof TITLES]
      if(Object.keys(group).length) card.append(groupBox(title, group))
    }
  }
}