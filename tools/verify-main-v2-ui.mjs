// Existing main web/index.html must run the latest C++ V2 binary.
import assert from "node:assert/strict";
import {readFileSync} from "node:fs";
const html=readFileSync(new URL("../web/index.html",import.meta.url),"utf8");
const js=readFileSync(new URL("../web/app-v2.js",import.meta.url),"utf8");
const binary=readFileSync(new URL("../web/slot-v2.wasm",import.meta.url));
const built=readFileSync(new URL("../src/slot-v2/wasm/slot-v2.wasm",import.meta.url));
assert.match(html,/src="app-v2\.js/);
assert.doesNotMatch(html,/src="slot\.js|src="app\.js/);
assert.match(js,/fetch\("slot-v2\.wasm/);
assert.equal(Buffer.compare(binary,built),0,"web WASM must match latest compiled C++");
assert(WebAssembly.validate(binary),"compiled WASM must validate");
const {instance}=await WebAssembly.instantiate(binary,{});
const e=instance.exports;
for(const key of ["slot_v2_reset","slot_v2_lever","slot_v2_stop","slot_v2_phase",
   "slot_v2_machine_area","slot_v2_normal_latent","slot_v2_entry_gate",
   "slot_v2_debug_arm","slot_v2_debug_count","slot_v2_debug_name_ptr",
   "slot_v2_test_bet","slot_v2_test_payout","slot_v2_test_bonus_gain",
   "slot_v2_at_table",
  "slot_v2_normal_flow","slot_v2_at_omen","slot_v2_special_zone_transition",
  "slot_v2_upper_special","slot_v2_chain_zone","slot_v2_at_stock_restart",
  "slot_v2_entry_gate_transition","slot_v2_at_cold"])assert.equal(typeof e[key],"function",key);
assert.equal(e.slot_v2_debug_count(),71);
e.slot_v2_reset(0x12341234,0);
assert.equal(e.slot_v2_setting(),6);
assert.equal(e.slot_v2_debug_arm(11,2),1,"configure CZ presentation");
assert.equal(e.slot_v2_debug_arm(3,2),1,"configure normal bonus");
function spin(){
   const before=e.slot_v2_phase();
   const result=e.slot_v2_lever();
   const after=e.slot_v2_phase();
   if ((result>>>24)!==0 || (after!==1 && after!==2)) {
       throw Error("lever did not start expected spin: before="+before+
         " result="+result+" after="+after+" area="+e.slot_v2_machine_area()+
         " pending="+e.slot_v2_pending_events());
   }
   if(after===2){e.slot_v2_complete_special();return}
   for(let reel=0;reel<3;reel++){
       let accepted=false;
       for(let pos=0;pos<21;pos++){
           const r=e.slot_v2_stop(reel,pos);
           if([0,5,6,7].includes((r>>>16)&255)){accepted=true;break}
       }
       assert(accepted,"accepted stop for reel "+reel);
   }
   assert.equal(e.slot_v2_phase(),3,"completed after three stops");
}
spin();
const first=e.slot_v2_normal_latent();
assert.equal(first&255,1,"normal hit should be concealed");
assert.equal(first>>>24,2,"forced 95% CZ presentation");
let waiting=0;
while((e.slot_v2_normal_latent()&255)===1 && waiting<26){spin();waiting++}
assert(waiting>=10&&waiting<=25,"omen lasts 10 to 25 spins, got "+waiting);
assert.equal(e.slot_v2_machine_area(),1,"scripted CZ begins after concealment");
console.log("MAIN V2 PLAYABLE PASS bytes="+binary.length+
   " flags="+e.slot_v2_debug_count()+" omen="+waiting+" CZ=1");
