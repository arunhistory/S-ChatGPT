import assert from "node:assert/strict";
import {readFileSync} from "node:fs";

const binary=readFileSync(new URL("../src/slot-v2/wasm/slot-v2.wasm",import.meta.url));
assert(WebAssembly.validate(binary),"WASM validates");
const {instance}=await WebAssembly.instantiate(binary,{});
const e=instance.exports;

const required=[
  "slot_v2_reset","slot_v2_lever","slot_v2_phase","slot_v2_preview_stop","slot_v2_stop",
  "slot_v2_visible_symbol","slot_v2_stopped_position","slot_v2_acquisition",
  "slot_v2_machine_area","slot_v2_entry_gate","slot_v2_bell_navigation_next",
  "slot_v2_lower_fall_challenge","slot_v2_lower_fall_push",
  "slot_v2_bonus_state","slot_v2_bonus_cycle","slot_v2_bonus_transition",
  "slot_v2_at_active","slot_v2_at_tier","slot_v2_section_diff",
  "slot_v2_bet","slot_v2_payout","slot_v2_bonus_net_gain","slot_v2_complete_special"
];
for(const k of required)assert.equal(typeof e[k],"function","missing "+k);

const ASSIST=new Set([3,4,5,12,13,14]);
const WATERMELON=8, STRONG_CHERRY=7, STRONG_CHANCE=10;
const SYM={BELL:4,REPLAY:5,CHERRY:6,WATERMELON:7,PENGUIN:8};

function u32(x){return Number(x)>>>0}
function targetPress(role,reel){
  const gate=u32(e.slot_v2_entry_gate());
  const kind=(gate>>>8)&255;
  if(gate&2){
    if(reel===0)return 4;
    if(reel===1)return 8;
    if(reel===2)return kind===2?11:12;
  }
  if(role===STRONG_CHERRY)return [14,8,0][reel]??0;
  if(role===STRONG_CHANCE)return [17,9,2][reel]??0;
  if(role===2&&reel===0)return 0;
  return 0;
}
function previewAcceptable(role,reel,preview){
  const status=(preview>>>16)&255, center=preview&255;
  if(ASSIST.has(role))return status===0;
  if(role===STRONG_CHERRY){
    if(status!==0)return false;
    if(reel===0&&u32(e.slot_v2_visible_symbol(0,center,0))!==SYM.CHERRY)return false;
    if(reel===1&&u32(e.slot_v2_visible_symbol(1,center,0))===SYM.REPLAY)return false;
    return true;
  }
  return status===0||status===5||status===6;
}
function commitReels(role,stats){
  const stopped=[false,false,false], statuses=[];
  for(let n=0;n<3;n++){
    const nav=u32(e.slot_v2_bell_navigation_next());
    let reel=(nav<3&&!stopped[nav])?nav:stopped.findIndex(x=>!x);
    assert(reel>=0,"a reel remains stoppable");
    const target=targetPress(role,reel);
    let chosen=-1,preview=0;
    for(let i=0;i<21;i++){
      const press=(target+i)%21;
      const p=u32(e.slot_v2_preview_stop(reel,press));
      if(previewAcceptable(role,reel,p)){chosen=press;preview=p;break}
    }
    assert.notEqual(chosen,-1,`no legal preview role=${role} reel=${reel}`);
    const committed=u32(e.slot_v2_stop(reel,chosen));
    assert.equal(committed&0xffff,preview&0xffff,"preview/commit stop differs");
    const status=(committed>>>16)&255;
    assert(previewAcceptable(role,reel,committed),`committed illegal status ${status}`);
    stopped[reel]=true;statuses[reel]=status;

    const center=committed&255;
    const sym=u32(e.slot_v2_visible_symbol(reel,center,0));
    if(role!==WATERMELON){
      assert.notEqual(sym,SYM.WATERMELON,
        `unrelated middle-line WATERMELON role=${role} reel=${reel}`);
    }
  }
  assert.equal(u32(e.slot_v2_phase()),3,"three stops complete the spin");

  if(role===STRONG_CHERRY){
    const lp=u32(e.slot_v2_stopped_position(0));
    const mp=u32(e.slot_v2_stopped_position(1));
    assert.equal(u32(e.slot_v2_visible_symbol(0,lp,0)),SYM.CHERRY,"strong cherry left middle");
    assert.notEqual(u32(e.slot_v2_visible_symbol(1,mp,0)),SYM.REPLAY,"strong cherry middle no replay");
  }
  if(role===STRONG_CHANCE&&statuses.every(x=>x===0)){
    const got=[0,1,2].map(r=>{
      const p=u32(e.slot_v2_stopped_position(r));
      return u32(e.slot_v2_visible_symbol(r,p,0));
    });
    assert.deepEqual(got,[SYM.PENGUIN,SYM.CHERRY,SYM.PENGUIN],"strong chance shape");
  }
  stats.stoppedSpins++;
}
function settle(areaAtStart){
  const acquisition=u32(e.slot_v2_acquisition());
  const status=acquisition&255, role=(acquisition>>>8)&255, award=(acquisition>>>16)&65535;
  let payout=status===1?award:0;
  if(role===2)payout=Math.max(1,payout);
  if(role===5&&status===1)payout=Math.max(3,payout);
  if(areaAtStart===3){
    const tier=u32(e.slot_v2_at_tier());
    const net=[6,6,9][tier]??6;
    payout=Math.max(payout,3+net);
  }
  if(areaAtStart===2)payout=Math.max(payout,13);
  if(payout)e.slot_v2_payout(payout);
  if(areaAtStart===2)e.slot_v2_bonus_net_gain(10);
}
function run(seed,games){
  e.slot_v2_reset(seed>>>0,Math.floor(seed/0x100000000)>>>0);
  const stats={
    seed,games:0,rejected:0,pushes:0,stoppedSpins:0,directSpecial:0,
    areas:[0,0,0,0,0],roles:Array(15).fill(0),bonusEnds:0,atBonusGames:0,
    maxAbsDiff:0n
  };
  let previousBonusEnd=false;
  let guard=0;
  while(stats.games<games){
    assert(++guard<games*20,`loop guard seed=${seed} games=${stats.games}`);

    if((u32(e.slot_v2_lower_fall_challenge())&255)===3){
      const pushed=u32(e.slot_v2_lower_fall_push());
      assert(pushed===1||pushed===2,"lower fall PUSH resolves");
      stats.pushes++;
      continue;
    }

    const areaBefore=u32(e.slot_v2_machine_area());
    const packed=u32(e.slot_v2_lever());
    const command=(packed>>>24)&255;
    if(command!==0){
      stats.rejected++;
      throw new Error(`unexpected lever rejection area=${areaBefore} phase=${u32(e.slot_v2_phase())} pending-state game=${stats.games}`);
    }

    // BONUS end/cycle fields are one-shot and must be gone as the next game starts.
    if(previousBonusEnd){
      assert.equal((u32(e.slot_v2_bonus_cycle())>>>16)&255,0,"stale BONUS cycle on next lever");
      assert.equal(u32(e.slot_v2_bonus_transition())&255,0,"stale BONUS transition on next lever");
      previousBonusEnd=false;
    }

    e.slot_v2_bet(3);
    const role=packed&255, special=(packed>>>8)&255;
    if(role<stats.roles.length)stats.roles[role]++;
    const phase=u32(e.slot_v2_phase());

    if(phase===2){
      e.slot_v2_complete_special();
      assert.equal(u32(e.slot_v2_phase()),3,"direct special completes");
      stats.directSpecial++;
    }else{
      assert.equal(phase,1,`accepted lever enters stoppable phase, got ${phase}`);
      commitReels(role,stats);
      settle(areaBefore);
    }

    const areaAfter=u32(e.slot_v2_machine_area());
    assert(areaAfter<stats.areas.length,"known machine area");
    stats.areas[areaAfter]++;
    const bonus=u32(e.slot_v2_bonus_state());
    if(areaAfter===2)assert(bonus&1,"BONUS area must have active BONUS");
    if(areaBefore===2&&u32(e.slot_v2_at_active()))stats.atBonusGames++;

    const bt=u32(e.slot_v2_bonus_transition())&255;
    if(bt===2){stats.bonusEnds++;previousBonusEnd=true}

    const d=BigInt(e.slot_v2_section_diff());
    const ad=d<0n?-d:d;
    if(ad>stats.maxAbsDiff)stats.maxAbsDiff=ad;
    stats.games++;
  }
  return stats;
}

const runs=[
  run(0x10001,25000),
  run(0x20002,25000),
  run(0x30003,25000),
  run(0x40004,25000)
];
const total=runs.reduce((a,r)=>{
  a.games+=r.games;a.rejected+=r.rejected;a.pushes+=r.pushes;
  a.stoppedSpins+=r.stoppedSpins;a.directSpecial+=r.directSpecial;
  a.bonusEnds+=r.bonusEnds;a.atBonusGames+=r.atBonusGames;
  for(let i=0;i<5;i++)a.areas[i]+=r.areas[i];
  for(let i=0;i<15;i++)a.roles[i]+=r.roles[i];
  return a;
},{games:0,rejected:0,pushes:0,stoppedSpins:0,directSpecial:0,bonusEnds:0,atBonusGames:0,areas:[0,0,0,0,0],roles:Array(15).fill(0)});

assert.equal(total.games,100000);
assert.equal(total.rejected,0,"no user-unresolvable lever deadlocks");
assert(total.areas[1]>0,"CZ exercised");
assert(total.areas[2]>0,"BONUS exercised");
assert(total.areas[3]>0,"AT exercised");
assert(total.areas[4]>0,"revival exercised");
assert(total.bonusEnds>0,"BONUS completion exercised");
assert(total.atBonusGames>0,"AT -> BONUS -> AT lifecycle exercised");

console.log("V2 WASM LONG-RUN PASS",JSON.stringify(total));
for(const r of runs)console.log("seed",r.seed,JSON.stringify({...r,maxAbsDiff:String(r.maxAbsDiff)}));
