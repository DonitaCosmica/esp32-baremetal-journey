export const renderState = (el, state) => {
    const led = state.led === 1 ? 'ON' : state.led === 0 ? 'OFF' : '—';
    el.textContent = `LED: ${led}` + (state.online ? '' : ' (dispositivo offline)');
};
export const renderDisconnected = (el) => {
    el.textContent = 'LED: sin conexión';
};
