const label = (r, on, off) => r === null ? '—' : r.value === 1 ? on : off;
const offline = (s) => (s.online ? '' : ' (dispositivo offline)');
export const renderLed = (el, s) => {
    el.textContent = `LED: ${label(s.led, 'ON', 'OFF')}${offline(s)}`;
};
export const renderProximity = (el, s) => {
    el.textContent = `Sensor: ${label(s.proximity, 'objeto detectado', 'objeto no detectado')}${offline(s)}`;
};
export const renderDisconnected = (el, name) => {
    el.textContent = `${name}: sin conexión`;
};
