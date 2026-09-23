// One-shot Node smoke test for the production v2 WASM, not the legacy engine.
import assert from "node:assert/strict";
import { readFileSync } from "node:fs";

const binary = readFileSync(new URL("../src/slot-v2/wasm/slot-v2.wasm", import.meta.url));
assert.equal(WebAssembly.validate(binary), true, "valid WASM binary");
const { instance } = await WebAssembly.instantiate(binary, {});
const e = instance.exports;

for (const key of ["slot_v2_reset","slot_v2_reset_setting","slot_v2_setting",
  "slot_v2_lever","slot_v2_stop","slot_v2_at_resolution",
  "slot_v2_special_zone","slot_v2_bonus_cycle"]) {
  assert.equal(typeof e[key], "function", `missing WASM export: ${key}`);
}

e.slot_v2_reset(0x1001,0);
assert.equal(e.slot_v2_setting(),6,"default setting must be 6");
for(const id of [1,2,3,4,5,7]) {
  assert.equal(e.slot_v2_reset_setting(0x1002,0,id),2,
    `setting ${id} must be a non-playable placeholder`);
  assert.equal(e.slot_v2_setting(),6,
    `placeholder ${id} must not alter current setting`);
}
assert.equal(e.slot_v2_reset_setting(0x1003,0,6),0);
assert.equal(e.slot_v2_setting(),6);
assert.equal(e.slot_v2_reset_setting(0x1004,0,0),1);
assert.equal(e.slot_v2_reset_setting(0x1005,0,8),1);

// Exercise lever + three reel stops over deterministic normal games.
// Some spins may enter an entitlement waiting state, and third-stop checks
// may reject invalid combinations; verify commands never throw/trap.
for (let spin=0;spin<10000;spin++) {
  const result=e.slot_v2_lever();
  if ((result>>>24)!==0) continue;
  const order=[e.slot_v2_stop(0,0),e.slot_v2_stop(1,0),e.slot_v2_stop(2,0)];
  assert.equal(order.length,3);
}
console.log(`WASM PASS bytes=${binary.byteLength} default-setting=6 placeholders-rejected lever-cycles=10000`);
