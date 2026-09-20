// Freestanding WebAssembly loader for S-ChatGPT SLOT.
// Generated runtime does not depend on Emscripten or GitHub Actions.
var createSlotModule = (function () {
  var scriptUrl = (typeof document !== 'undefined' && document.currentScript && document.currentScript.src)
    ? document.currentScript.src
    : (typeof location !== 'undefined' ? location.href : '');

  return async function createSlotModule() {
    var baseUrl = new URL('.', scriptUrl);
    var manifestUrl = new URL('slot.wasm.b64.manifest', baseUrl);
    var manifestResponse = await fetch(manifestUrl, { credentials: 'same-origin' });
    if (!manifestResponse.ok) throw new Error('WASM manifest load failed: ' + manifestResponse.status);

    var chunkCount = Number((await manifestResponse.text()).trim());
    if (!Number.isInteger(chunkCount) || chunkCount < 1 || chunkCount > 128) {
      throw new Error('Invalid WASM chunk manifest');
    }

    var chunkRequests = [];
    for (var n = 1; n <= chunkCount; n++) {
      (function (chunkIndex) {
        var chunkUrl = new URL('slot.wasm.b64.' + chunkIndex, baseUrl);
        chunkRequests.push(fetch(chunkUrl, { credentials: 'same-origin' }).then(async function (response) {
          if (!response.ok) throw new Error('WASM chunk ' + chunkIndex + ' load failed: ' + response.status);
          return (await response.text()).trim();
        }));
      })(n);
    }

    var encoded = (await Promise.all(chunkRequests)).join('');
    var binary = atob(encoded);
    var bytes = new Uint8Array(binary.length);
    for (var i = 0; i < binary.length; i++) bytes[i] = binary.charCodeAt(i);

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
