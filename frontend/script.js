const DEVICES = [
  { id: 'esp32-s3', url: 'http://localhost:3000/api/s3/led' }
]

const refresh = async ({ id, url }) => {
  const element = document.getElementById(id)

  try {
    const res = await fetch(url, { cache: 'no-store' })
    if (!res.ok) throw new Error(res.statusText)

    const data = await res.json()
    const led = data.led === 1 ? 'ON' : data.led === 0 ? 'OFF' : '—'
    element.textContent = `LED: ${ led }` + (data.online ? '' : ' (dispositivo offline)')
  } catch (error) {
    element.textContent = 'LED: sin conexión'
  }
}

DEVICES.forEach(device => {
  refresh(device)
  setInterval(() => refresh(device), 1000)
})