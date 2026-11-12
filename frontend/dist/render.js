const TITLES = {
    metrics: 'Mediciones',
    sensors_health: 'Salud de sensores',
    diagnostics: 'Diagnóstico',
};
const FORMAT = {
    led: (value) => (value ? 'ON' : 'OFF'),
    object_detected: (value) => (value ? 'objeto detectado' : 'objeto no detectado'),
};
const cards = new Map();
const element = (tag, cls, text) => {
    const el = document.createElement(tag);
    if (cls)
        el.className = cls;
    if (text !== undefined)
        el.textContent = text;
    return el;
};
const groupBox = (title, group) => {
    const box = element('div', 'group');
    box.append(element('h4', undefined, title));
    for (const [key, value] of Object.entries(group)) {
        const row = element('div', 'row');
        row.append(element('span', 'k', key), element('span', 'v', FORMAT[key]?.(value) ?? String(value)));
        box.append(row);
    }
    return box;
};
const getCard = (root, key) => {
    const existing = cards.get(key);
    if (existing)
        return existing;
    const created = element('section', 'card');
    cards.set(key, created);
    root.append(created);
    return created;
};
export const renderDevice = (root, device) => {
    const card = getCard(root, device.key);
    const head = element('div', 'head');
    head.append(element('strong', undefined, device.id), element('span', device.online ? 'badge on' : 'badge off', device.online ? 'online' : 'offline'));
    card.replaceChildren(head, element('small', undefined, `${device.region} / ${device.zone}`));
    if (device.telemetry) {
        for (const [k, title] of Object.entries(TITLES)) {
            const group = device.telemetry[k];
            if (Object.keys(group).length)
                card.append(groupBox(title, group));
        }
    }
};
