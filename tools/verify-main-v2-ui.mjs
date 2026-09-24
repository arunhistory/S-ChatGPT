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
for(const key of ["slot_v2_reset","slot_v2_lever","slot_v2_stop","slot_v2_preview_stop","slot_v2_phase",
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
function commitPreviewedStop(reel, statuses=[0,5,6]){
   let chosen=-1;
   for(let pos=0;pos<21;pos++){
       const p=e.slot_v2_preview_stop(reel,pos)>>>0;
       if(statuses.includes((p>>>16)&255)){chosen=pos;break}
   }
   assert.notEqual(chosen,-1,"preview found legal stop for reel "+reel);
   const r=e.slot_v2_stop(reel,chosen)>>>0;
   assert(statuses.includes((r>>>16)&255),"committed previewed stop for reel "+reel);
   return r;
}
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
   for(let reel=0;reel<3;reel++)commitPreviewedStop(reel,[0,5,6]);
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

// Reel-result invariants: internal flag and visible middle line must agree.
function forceRole(role, requiredStatuses=[0]){
   e.slot_v2_reset(0x77aa55cc,0);
   assert.equal(e.slot_v2_debug_arm(1,role),1,"arm role "+role);
   const result=e.slot_v2_lever()>>>0;
   assert.equal(result&255,role,"forced role "+role);
   for(let reel=0;reel<3;reel++)commitPreviewedStop(reel,requiredStatuses);
}
const center=(reel)=>e.slot_v2_visible_symbol(reel,e.slot_v2_stopped_position(reel),0);

// 9枚ベル = 中段有効ライン。左・中に🍉を残さない。
forceRole(3,[0]);
assert.equal(center(0),4,"9-medal bell left middle must be BELL");
assert.equal(center(1),4,"9-medal bell middle middle must be BELL");
assert([1,4].includes(center(2)),"9-medal bell right middle must be BELL or red-7 assist");

// 強チェリー = 左中段🍒、中段REPLAYなし。
forceRole(7,[0]);
assert.equal(center(0),6,"strong cherry must stop CHERRY on left middle");
assert.notEqual(center(1),5,"strong cherry must not show REPLAY on middle middle");

// 強チャンス目 = 🐧 / 🍒 / 🐧。
forceRole(10,[0]);
assert.equal(center(0),8,"strong chance left middle PENGUIN");
assert.equal(center(1),6,"strong chance middle middle CHERRY");
assert.equal(center(2),8,"strong chance right middle PENGUIN");

// 1枚役では有効ラインに🍉を出さない。
forceRole(2,[0]);
assert.notEqual(center(0),7,"one-medal left middle must not be WATERMELON");
assert.notEqual(center(1),7,"one-medal middle middle must not be WATERMELON");
assert.notEqual(center(2),7,"one-medal right middle must not be WATERMELON");

assert.match(js,/PREMIUM FREEZE/);
assert.match(js,/startReverseVisualSpin/);
assert.match(js,/右リールの 🟥7 \/ BAR で行き先決定/);
assert.match(js,/entryJudge\?"開始図柄"/);
assert.match(js,/entryJudge\?"entry_judge"/);
assert.match(js,/非表示（停止結果待ち）/);
assert.match(js,/右リールで決まる…/);
assert.match(js,/気配がする…/);
assert.match(js,/strongCherryPress=\[14,8,0\]/);
assert.match(js,/currentRole===7/);
assert.match(js,/visible_symbol\(0,center,0\)\)!==6/);
assert.match(js,/strongChancePress=\[17,9,2\]/);
assert.match(js,/One-medal explicitly anchors the left reel on BAR/);
assert.match(js,/EPISODE BONUS/);
assert.match(js,/上位特化ZONE/);
assert.match(js,/CHAIN ZONE/);
assert.match(js,/64G 引き戻し/);
assert.match(js,/復活チャレンジ/);
assert.match(js,/高確率 移行/);
assert.match(js,/G数短縮/);
assert.match(js,/STOCK獲得/);
assert.match(js,/有利区間 CROSS/);
assert.match(js,/slot_v2_preview_stop/);
assert.match(js,/commit exactly one real STOP/);
assert.match(js,/\[3,4,5,12,13,14\]\.includes\(currentRole\)/);
assert.match(js,/playPresentationEvents/);
