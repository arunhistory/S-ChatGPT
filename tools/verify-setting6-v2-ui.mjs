// Verify the actual test UI uses latest native WASM and reaches CZ
// after completing the full 10-25 game omen. Does not use v1 engine.
import assert from "node:assert/strict";
import { readFileSync } from "node:fs";
const html=readFileSync(new URL("../web/setting6-v2.html",import.meta.url),"utf8");
const script=readFileSync(new URL("../web/setting6-v2.js",import.meta.url),"utf8");
const binary=readFileSync(new URL("../web/slot-v2.wasm",import.meta.url));
const built=readFileSync(new URL("../src/slot-v2/wasm/slot-v2.wasm",import.meta.url));
assert.equal(Buffer.compare(binary,built),0,"page WASM must match rebuilt C++ WASM");
assert.match(html,/setting6-v2\.js/);
assert.doesNotMatch(html,/src="slot\.js|src="app\.js/);
assert.match(script,/fetch\("slot-v2\.wasm/);
assert.doesNotMatch(script,/createSlotModule|fetch\("slot\.wasm/);
assert.equal(WebAssembly.validate(binary),true);
const {instance}=await WebAssembly.instantiate(binary,{});
const e=instance.exports;
for (const key of ["slot_v2_reset","slot_v2_setting","slot_v2_lever",
   "slot_v2_stop","slot_v2_machine_area","slot_v2_normal_latent",
   "slot_v2_entry_gate","slot_v2_debug_arm","slot_v2_debug_count",
   "slot_v2_debug_channel","slot_v2_debug_value","slot_v2_debug_name_ptr",
   "slot_v2_test_bet","slot_v2_test_payout","slot_v2_test_bonus_gain",
   "slot_v2_at_table"]) assert.equal(typeof e[key],"function",key);
assert.equal(e.slot_v2_debug_count(),70);
e.slot_v2_reset(0x33334444,0);
assert.equal(e.slot_v2_setting(),6);
assert.equal(e.slot_v2_debug_arm(11,2),1,"set 95% presentation route");
assert.equal(e.slot_v2_debug_arm(3,2),1,"force a normal bonus internally");
function spin(){
  const r=e.slot_v2_lever();
  assert.equal(r>>>24,0,"lever accepted");
  assert.equal(e.slot_v2_phase(),1);
  for(let i=0;i<3;i++){
    let found=false;
    for(let p=0;p<21;p++){
      const res=e.slot_v2_stop(i,p);
      if([0,5,6,7].includes((res>>>16)&255)){found=true;break;}
    }
    assert(found,"C++ reel stop "+i);
  }
  assert.equal(e.slot_v2_phase(),3);
}
spin();
assert.equal(e.slot_v2_normal_latent()&255,1,"normal hit concealed after first spin");
assert.equal(e.slot_v2_normal_latent()>>>24,2,"CZ route preserved");
assert.equal(e.slot_v2_entry_gate()&1,0,"entry signal must not start immediately");
let omen=0;
while((e.slot_v2_normal_latent()&255)===1 && omen<26){spin();omen++;}
assert(omen>=10&&omen<=25,"latent 10-25 games");
assert.equal(e.slot_v2_machine_area(),1,"CZ starts after omen");
console.log("V2 BROWSER CONTRACT PASS bytes="+binary.length+" flags=70 omen="+omen+" CZ=1");
