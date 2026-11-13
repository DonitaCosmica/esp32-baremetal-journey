const parse = (event) => {
    const data = JSON.parse(event.data);
    console.log('API Data received:', data);
    return data;
};
export const watchDevices = (url, onSnapshot, onDevice, onConnection) => {
    const source = new EventSource(url);
    source.onopen = () => onConnection(true);
    source.onerror = () => onConnection(false);
    source.addEventListener('snapshot', (event) => onSnapshot(parse(event)));
    source.addEventListener('device', (event) => onDevice(parse(event)));
    return () => source.close();
};
