import {readFileSync} from "node:fs";
const file=readFileSync("./src/slot-v2/wasm/slot-v2.wasm");
const {instance}=await WebAssembly.instantiate(file,{});
let e=instance.exports;
function log(label) {console.log(label,JSON.stringify({phase:e.slot_v2_phase(),area:e.slot_v2_machine_area(),entry:e.slot_v2_entry_gate(),normal:e.slot_v2_normal_latent(),pending:e.slot_v2_pending_events(),lastRole:e.slot_v2_last_role(),setting:e.slot_v2_setting()}));}
e.slot_v2_reset(0x12341234,0);log("RESET");
let r=e.slot_v2_lever();console.log("BASE LEVER",r,e.slot_v2_phase());log("BASE LEVER STATE");
console.log("BASE STOP",e.slot_v2_stop(0,0),e.slot_v2_phase());
e.slot_v2_reset(0x12341234,0);log("RESET2");
console.log("route arm",e.slot_v2_debug_arm(11,2),"bonus arm",e.slot_v2_debug_arm(3,2));
r=e.slot_v2_lever();console.log("DEBUG LEVER",r,e.slot_v2_phase());log("DEBUG LEVER STATE");
console.log("DEBUG STOP",e.slot_v2_stop(0,0),e.slot_v2_phase());