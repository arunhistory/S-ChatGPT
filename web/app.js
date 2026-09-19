(async () => {
  const stateEl = document.querySelector('#state');
  const eventsEl = document.querySelector('#events');
  const Module = await createSlotModule();

  const getString = (name) => Module.UTF8ToString(Module.ccall(name, 'number', [], []));
  const refresh = () => stateEl.textContent = JSON.stringify(JSON.parse(getString('slot_state_json')), null, 2);
  const act = (fn) => {
    const data = JSON.parse(getString(fn));
    eventsEl.textContent = JSON.stringify(data, null, 2);
    refresh();
  };

  document.querySelector('#normal').onclick = () => act('slot_spin_normal_json');
  document.querySelector('#at').onclick = () => act('slot_spin_at_json');
  document.querySelector('#reset').onclick = () => {
    const seed = BigInt(Date.now());
    Module.ccall('slot_reset', null, ['number','number'], [Number(seed & 0xffffffffn), Number(seed >> 32n)]);
    eventsEl.textContent = '-';
    refresh();
  };
  refresh();
})();
