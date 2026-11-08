"use strict";
const API = 'http://localhost:3000';
const DEVICES = [
    { id: 'esp32-s3', url: `${API}/api/s3/led/stream` }
];
const render = (element, state) => {
    const led = state.led === 1 ? 'ON' : state.led === 0 ? 'OFF' : '—';
    element.textContent = `LED: ${led}` + (state.online ? '' : ' (dispositivo offline)');
};
const connect = ({ id, url }) => {
    const element = document.getElementById(id);
    if (!element) {
        console.error(`No existe el elemento #${id}`);
        return;
    }
    const source = new EventSource(url);
    source.onmessage = (event) => {
        try {
            render(element, JSON.parse(event.data));
        }
        catch {
            console.error('Evento invalido: ', event.data);
        }
    };
    source.onerror = () => {
        element.textContent = 'LED: sin conexion';
    };
};
DEVICES.forEach(connect);
