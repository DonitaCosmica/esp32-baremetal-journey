export const watchDevice = (url, onState, onError) => {
    const source = new EventSource(url);
    source.onmessage = (event) => {
        try {
            onState(JSON.parse(event.data));
        }
        catch {
            console.error('Evento inválido:', event.data);
        }
    };
    source.onerror = onError;
    return () => source.close();
};
