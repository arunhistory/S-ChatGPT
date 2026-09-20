// Freestanding WebAssembly loader for S-ChatGPT SLOT.
// Generated runtime does not depend on Emscripten or GitHub Actions.
var createSlotModule = (function () {
  var scriptUrl = (typeof document !== 'undefined' && document.currentScript && document.currentScript.src)
    ? document.currentScript.src
    : (typeof location !== 'undefined' ? location.href : '');

  return async function createSlotModule() {
    var wasmUrl = new URL('slot.wasm', new URL('.', scriptUrl));
    var response = await fetch(wasmUrl, { credentials: 'same-origin' });
    if (!response.ok) throw new Error('slot.wasm load failed: ' + response.status);

    var bytes = await response.arrayBuffer();
    var result = await WebAssembly.instantiate(bytes, {});
    var instance = result.instance;
    var ex = instance.exports;
    var decoder = new TextDecoder('utf-8');

    function UTF8ToString(ptr) {
      if (!ptr) return '';
      var memory = new Uint8Array(ex.memory.buffer);
      var end = ptr;
      while (end < memory.length && memory[end] !== 0) end++;
      return decoder.decode(memory.subarray(ptr, end));
    }

    function ccall(name, returnType, argTypes, args) {
      var fn = ex[name];
      if (typeof fn !== 'function') throw new Error('Missing WASM export: ' + name);
      var ret = fn.apply(null, args || []);
      if (returnType === 'string') return UTF8ToString(ret);
      if (returnType === 'boolean') return Boolean(ret);
      return ret;
    }

    return {
      ccall: ccall,
      UTF8ToString: UTF8ToString,
      wasmInstance: instance,
      memory: ex.memory
    };
  };
})();
