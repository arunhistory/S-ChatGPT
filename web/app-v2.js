(() => {
  "use strict";
  const $ = id => document.getElementById(id);
  let e = null, memory = null, engineReady = false, auto = false, timer = null;
  let totalSpins = 0, startedArea = 0, currentRole = 0, currentSpecial = 0, lastWin = 0, credits = 1000;
  let presentationLock = false;
  const presentationTimers = [];
  const stopped = [true,true,true], centers = [0,0,0], history = [], diff = [0];
  const reelSpinTimers = [null,null,null];
  const reelVisualPositions = [0,7,14];
  let prev = { area:0, gate:0, latent:0 }, startedSnapshot = null;
  const ROLE = ["なし","ハズレ","1枚役","9枚ベル","15枚ベル","リプレイ",
     "弱チェリー","強チェリー","スイカ","弱チャンス目","強チャンス目",
     "ペンギンチャンス","AT開始7","BONUS開始7","3枚役"];
  const SYM = ["?","7","7","BAR","🔔","↻","🍒","🍉","🐧","❄️"];
  const KIND = ["unknown","seven","alt-seven","bar","bell","replay","cherry","watermelon","penguin","miss"];
  const ROLE_KEY = ["none","miss","one_medal","bell9","bell15","replay",
    "weak_cherry","strong_cherry","watermelon","weak_chance","strong_chance",
    "penguin_chance","at","hit","three_medal"];
  const CHANNEL = {"1":"小役・開始図柄","2":"特殊直撃","3":"通常抽選","4":"高確抽選",
    "5":"レア役当選","6":"天井","7":"AT内部当選","8":"CZ結果",
    "9":"内部Pendingフラグ","10":"通常モード","11":"潜伏経路"};
  const TRANSLATE = {
    "RoleFlag::None":"なし","RoleFlag::Miss":"ハズレ","RoleFlag::OneMedal":"1枚役",
    "RoleFlag::Bell9":"9枚ベル（上・中・下段）","RoleFlag::Bell15":"15枚ベル（右上がり）","RoleFlag::ThreeMedal":"3枚役（右下がり）",
    "RoleFlag::Replay":"リプレイ","RoleFlag::WeakCherry":"弱チェリー",
    "RoleFlag::StrongCherry":"強チェリー","RoleFlag::Watermelon":"スイカ",
    "RoleFlag::WeakChance":"弱チャンス目","RoleFlag::StrongChance":"強チャンス目",
    "RoleFlag::PenguinChance":"ペンギンチャンス","RoleFlag::EntryAT":"AT開始図柄",
    "RoleFlag::EntryBonus":"BONUS開始図柄",
    "SpecialHit::MiddleATStock":"中位AT＋ストック直撃",
    "SpecialHit::UpperAT":"上位AT直撃","SpecialHit::Freeze":"フリーズ",
    "normal_flow::Reward::LowerAT":"通常抽選・下位AT",
    "normal_flow::Reward::Bonus":"通常抽選・BONUS",
    "normal_flow::Reward::CZ":"通常抽選・CZ",
    "normal_high::Reward::CZ":"高確・CZ","normal_high::Reward::Bonus":"高確・BONUS",
    "normal_role_trigger::DrawResult::Bonus":"レア役・BONUS",
    "normal_role_trigger::DrawResult::LowerAT":"レア役・下位AT",
    "normal_role_trigger::DrawResult::MiddleAT":"レア役・中位AT",
    "normal_ceiling::Reward::CZ":"天井・CZ","normal_ceiling::Reward::Bonus":"天井・BONUS",
    "normal_ceiling::Reward::LowerAT":"天井・下位AT",
    "normal_ceiling::Reward::LowerATWithStock":"天井・AT＋ストック",
    "normal_ceiling::Reward::Freeze":"天井・フリーズ",
    "normal_latent::Route::NormalEvent":"通常イベント潜伏",
    "normal_latent::Route::Omen":"通常予兆","normal_latent::Route::OmenCZ":"予兆→CZ"
  };
  const u8 = x => Number(x) & 255;
  const u32 = x => Number(x) >>> 0;
  const fmt = x => (Number(x)>=0?"+":"") + Number(x).toLocaleString("ja-JP");
  function say(title, note, cls="") {
    $("eventTitle").textContent = title;
    $("eventNote").textContent = note;
    $("eventBanner").className = "event-banner " + cls;
  }
  function log(s) {
    history.unshift({spin:totalSpins,text:s});
    if(history.length>70)history.pop();
    const root=$("history");root.replaceChildren();
    for(const item of history.slice(0,25)){
      const li=document.createElement("li"),b=document.createElement("b"),span=document.createElement("span");
      b.textContent=item.spin+"G";span.textContent=item.text;
      li.append(b,span);root.append(li);
    }
  }
  const machine=()=>document.querySelector(".machine");
  const stage=()=>$("stageScreen");
  const effect=()=>$("effectLayer");
  const cutin=()=>$("cutinLayer");
  const later=(fn,ms)=>{const id=setTimeout(fn,ms);presentationTimers.push(id);return id;};
  function clearPresentationTimers(){
    while(presentationTimers.length) clearTimeout(presentationTimers.pop());
    presentationLock=false;
  }
  function showMachineCinematic(eyebrow,title,sub,cls=""){
    const box=$("machineCinematic");
    box.className="machine-cinematic show "+cls;
    $("cinematicEyebrow").textContent=eyebrow||"";
    $("cinematicTitle").textContent=title||"";
    $("cinematicSub").textContent=sub||"";
  }
  function hideMachineCinematic(){
    const box=$("machineCinematic");
    box.className="machine-cinematic";
    $("cinematicEyebrow").textContent="";
    $("cinematicTitle").textContent="";
    $("cinematicSub").textContent="";
  }
  function clearStageClasses(){
    effect().className="effect-layer";
    cutin().className="cutin-layer";
    stage().classList.remove(
      "role-penguin","role-strong","role-hit","freeze-lock",
      "omen-white","omen-blue","omen-yellow","omen-green","omen-purple",
      "omen-red","omen-gold","omen-premium","screen-blackout","screen-shock"
    );
    $("omenText").className="omen-text";
    $("omenText").textContent="";
  }
  function choosePresentation(role,phase){
    // Entry destination is fixed internally, but presentation stays neutral until the reels reveal it.
    if(role==="entry_judge")return{cls:"omen-purple",fx:"purple",text:"どっちだ…"};
    if(role==="freeze")return{cls:"omen-premium screen-blackout",fx:"stripe flash rainbow",text:"……"};
    if(role==="at"||role==="tier_up")return{cls:"omen-gold screen-shock",fx:"gold flash",text:"激熱"};
    if(role==="hit")return{cls:"omen-red screen-shock",fx:"red flash slash",text:"好機"};
    if(role==="strong_cherry"||role==="strong_chance")
      return Math.random()<.25?{cls:"omen-gold screen-shock",fx:"gold slash",text:"激熱"}:{cls:"omen-red screen-shock",fx:"red slash",text:"チャンス"};
    if(role==="penguin_chance")return{cls:"omen-red screen-shock",fx:"red flash",text:"PENGUIN"};
    if(role==="weak_chance"){
      const r=Math.random();return r<.15?{cls:"omen-red",fx:"red",text:"チャンス"}:r<.55?{cls:"omen-purple",fx:"purple",text:"気配"}:{cls:"omen-blue",fx:"blue",text:""};
    }
    if(role==="weak_cherry")return Math.random()<.20?{cls:"omen-yellow",fx:"yellow",text:"気配"}:{cls:"omen-green",fx:"green",text:""};
    if(role==="watermelon")return Math.random()<.18?{cls:"omen-yellow",fx:"yellow",text:"気配"}:{cls:"omen-green",fx:"green",text:""};
    if(role==="bell15")return Math.random()<.12?{cls:"omen-yellow",fx:"yellow",text:""}:{cls:"",fx:"",text:""};
    if(role==="replay")return Math.random()<.035?{cls:"omen-white",fx:"flash",text:"…"}:{cls:"",fx:"",text:""};
    if(phase==="spin"&&Math.random()<.018)return{cls:"omen-white",fx:"",text:"…"};
    return{cls:"",fx:"",text:""};
  }
  function stageCue(role,phase="result"){
    clearStageClasses();
    const p=choosePresentation(role,phase);
    if(p.cls)stage().classList.add(...p.cls.split(" "));
    if(p.fx)effect().className="effect-layer "+p.fx;
    if(p.text){$("omenText").textContent=p.text;$("omenText").classList.add("show")}
    const cue={
      entry_judge:{fx:"purple",title:"",sub:"",eye:""},
      one_medal:{fx:"",title:"1枚役",sub:"静かな払い出し",eye:"NORMAL"},
      bell9:{fx:"yellow",title:"BELL",sub:"9枚",eye:"YELLOW"},
      bell15:{fx:"gold",title:"BIG BELL",sub:"15枚",eye:"GOLD"},
      replay:{fx:"blue",title:"REPLAY",sub:"もう一度",eye:"BLUE"},
      three_medal:{fx:"",title:"3枚役",sub:"右下がり 3枚",eye:"NORMAL"},
      weak_cherry:{fx:"green",title:"CHERRY",sub:"弱チェリー",eye:"GREEN"},
      strong_cherry:{fx:"red slash",title:"強チェリー",sub:"中段チェリー",eye:"RED",cls:"role-strong"},
      weak_chance:{fx:"purple",title:"CHANCE",sub:"弱チャンス目",eye:"PURPLE"},
      strong_chance:{fx:"red slash",title:"強チャンス目",sub:"🐧 🍒 🐧",eye:"RED",cls:"role-strong"},
      penguin_chance:{fx:"red",title:"PENGUIN CHANCE",sub:"🐧 🐧 🐧",eye:"RED",cls:"role-penguin"},
      hit:{fx:"red flash",title:"HIT",sub:"🟥7 🟥7 BAR",eye:"RED",cls:"role-hit"},
      at:{fx:"gold flash",title:"AT START",sub:"🟥7 🟥7 🟥7",eye:"GOLD",cls:"role-hit"},
      tier_up:{fx:"gold",title:"AT 昇格",sub:"TIER UP",eye:"GOLD",cls:"role-hit"},
      freeze:{fx:"stripe flash",title:"FREEZE",sub:"🟦7 🟦7 🟦7",eye:"PREMIUM",cls:"freeze-lock"},
      watermelon:{fx:"green",title:"WATERMELON",sub:"スイカ",eye:"GREEN"},
      miss:{fx:"",title:"",sub:"",eye:""}
    }[role]||{fx:"",title:"",sub:"",eye:""};
    if(phase==="spin"){
      if(role==="freeze")$("stageCaption").textContent="……";
      else if(role==="entry_judge")$("stageCaption").textContent="右リールで決まる…";
      else if(["strong_cherry","strong_chance","penguin_chance","hit","at","tier_up"].includes(role))
        $("stageCaption").textContent="気配がする…";
      return;
    }
    if(phase==="result"){
      if(cue.cls)stage().classList.add(...cue.cls.split(" "));
      if(cue.fx&&!p.fx)effect().className="effect-layer "+cue.fx;
      if(role!=="miss"&&role!=="none"&&role!=="entry_judge"){
        $("cutinEyebrow").textContent=cue.eye;$("cutinTitle").textContent=cue.title;$("cutinSub").textContent=cue.sub;
        void cutin().offsetWidth;cutin().classList.add("show");
      }
    }
  }
  function showEventCinematic(eye,title,sub,cls=""){
    showMachineCinematic(eye,title,sub,cls);
    later(hideMachineCinematic,900);
  }
  function startReverseVisualSpin(i){
    stopVisualSpin(i);
    const reel=$("reel"+(i+1));
    reel.classList.add("spinning","reverse-spinning");
    reelSpinTimers[i]=setInterval(()=>{
      reelVisualPositions[i]=(reelVisualPositions[i]+20)%21;
      reelRender(i,reelVisualPositions[i]);
    },54);
  }
  function runFreezeEntrySequence(){
    presentationLock=true;
    machine().classList.add("freeze-all-off");
    say("……","","premium");
    clearStageClasses();hideMachineCinematic();
    stopped.fill(false);
    for(let i=0;i<3;i++)startReverseVisualSpin(i);
    refresh();
    later(()=>{
      machine().classList.remove("freeze-all-off");
      machine().classList.add("freeze-return");
      showMachineCinematic("SYSTEM","PREMIUM FREEZE","異常回転","freeze");
      stage().classList.add("freeze-lock","omen-premium");
      effect().className="effect-layer stripe flash rainbow";
    },700);
    later(()=>{
      stopAllVisualSpins();
      for(let i=0;i<3;i++)startVisualSpin(i);
      showMachineCinematic("TARGET","🟦7を狙え","🟦7 🟦7 🟦7","freeze target");
      say("🟦7を狙え","3停止でFREEZE確定","premium");
    },1500);
    later(()=>{
      machine().classList.remove("freeze-return");
      presentationLock=false;
      refresh();scheduleAuto();
    },2250);
  }
  function runEntryGateCinematic(){
    presentationLock=true;
    showMachineCinematic("HIT","当たり","行き先はまだ分からない","judge");
    say("当たり","図柄告知待機","hot");
    later(()=>{
      showMachineCinematic("TARGET","🟥7を狙え","右リールの 🟥7 / BAR で行き先決定","judge target");
      say("🟥7を狙え","右リールでAT / BONUSをジャッジ","hot");
      presentationLock=false;refresh();scheduleAuto();
    },650);
  }
  function runDirectSpecialSequence(old,special){
    presentationLock=true;
    const title=special===1?"中位AT＋STOCK":special===2?"上位AT":"SPECIAL";
    const cls=special===2?"gold":"red";
    showMachineCinematic("DIRECT HIT",title,"特殊直撃",cls+" confirmed");
    say(title,"特殊直撃成立","premium");
    later(()=>{
      e.slot_v2_complete_special();
      totalSpins++;stopped.fill(true);lastWin=0;
      diff.push(Number(e.slot_v2_section_diff()));
      presentationLock=false;
      updateEvent(old,snapshot());
      refresh();scheduleAuto();
    },850);
  }

  function readCString(ptr) {
    const buf = new Uint8Array(memory.buffer);
    const end = Math.min(buf.length,ptr+160);
    let q=ptr;while(q<end && buf[q]!==0)q++;
    return new TextDecoder().decode(buf.subarray(ptr,q));
  }
  function populateFlags() {
    const root=$("flagPicker");root.replaceChildren();
    const empty=document.createElement("option");empty.value="";empty.textContent="通常C++抽選（選択なし）";root.append(empty);
    const groups={};
    for(let i=0;i<e.slot_v2_debug_count();i++){
      const channel=e.slot_v2_debug_channel(i),value=e.slot_v2_debug_value(i);
      const name=readCString(e.slot_v2_debug_name_ptr(i));
      const group=groups[channel]||(groups[channel]=document.createElement("optgroup"));
      group.label=CHANNEL[channel]||("CHANNEL "+channel);
      const option=document.createElement("option");
      option.value=channel+":"+value;option.textContent=TRANSLATE[name]||name.split("::").pop();
      group.append(option);
    }
    for(let channel=1;channel<=11;channel++)if(groups[channel])root.append(groups[channel]);
    $("debugCount").textContent=String(e.slot_v2_debug_count());
    if(root.querySelectorAll("option").length!==e.slot_v2_debug_count()+1)
      throw Error("デバッグフラグ一覧の件数が一致しません");
  }
  function reelRender(i,pos){
    const reel=$("reel"+(i+1));reel.replaceChildren();
    for(let off=-1;off<=1;off++){
      const symbol=e.slot_v2_visible_symbol(i,pos,off);
      const span=document.createElement("span");span.dataset.kind=KIND[symbol]||"miss";
      if(symbol===1){span.textContent="7";}else if(symbol===2){
        span.textContent="7";const small=document.createElement("small");small.textContent="BLUE";span.append(small);
      }else span.textContent=SYM[symbol]||"?";
      reel.append(span);
    }
  }
  function stopVisualSpin(i) {
    if(reelSpinTimers[i]!==null){
      clearInterval(reelSpinTimers[i]);
      reelSpinTimers[i]=null;
    }
    $("reel"+(i+1)).classList.remove("spinning","reverse-spinning");
  }
  function startVisualSpin(i) {
    stopVisualSpin(i);
    let pos=Number.isInteger(centers[i])?centers[i]:reelVisualPositions[i];
    reelVisualPositions[i]=pos;
    const reel=$("reel"+(i+1));
    reel.classList.add("spinning");
    reelSpinTimers[i]=setInterval(()=>{
      // Visual belt only. Stop result itself always comes from C++.
      reelVisualPositions[i]=(reelVisualPositions[i]+1)%21;
      reelRender(i,reelVisualPositions[i]);
    },54);
  }
  function stopAllVisualSpins() {
    for(let i=0;i<3;i++) stopVisualSpin(i);
  }

  function drawGraph() {
    const canvas=$("diffGraph");if(!canvas)return;
    const box=canvas.getBoundingClientRect(),w=Math.max(140,box.width),h=Math.max(60,box.height);
    const scale=window.devicePixelRatio||1;canvas.width=Math.floor(w*scale);canvas.height=Math.floor(h*scale);
    const c=canvas.getContext("2d");c.scale(scale,scale);c.clearRect(0,0,w,h);
    const max=Math.max(30,...diff),min=Math.min(-30,...diff),range=max-min;
    const y=x=>h-5-((x-min)/range)*(h-10);
    c.strokeStyle="#59739e";c.lineWidth=1;c.beginPath();c.moveTo(0,y(0));c.lineTo(w,y(0));c.stroke();
    if(diff.length<2)return;
    c.strokeStyle=diff[diff.length-1]>=0?"#67edb4":"#ffa474";c.lineWidth=2;c.beginPath();
    const arr=diff.slice(-250);
    arr.forEach((x,i)=>{const xx=(i/(arr.length-1))*w,yy=y(x);if(i===0)c.moveTo(xx,yy);else c.lineTo(xx,yy)});
    c.stroke();
  }
  function snapshot(){
    const area=e.slot_v2_machine_area(),gate=e.slot_v2_entry_gate(),latent=e.slot_v2_normal_latent();
    return {area,gate,latent,
       at:e.slot_v2_at_active(),tier:e.slot_v2_at_tier(),left:e.slot_v2_at_games_left(),
       stock:e.slot_v2_stock_count(),bonus:e.slot_v2_bonus_state(),
       cz:e.slot_v2_cz_cycle(),phase:e.slot_v2_phase(),
       normalFlow:e.slot_v2_normal_flow(),atOmen:e.slot_v2_at_omen(),
       sectionReward:e.slot_v2_section_reward(),sectionTransition:e.slot_v2_section_transition(),
       specialZone:e.slot_v2_special_zone(),specialZoneTransition:e.slot_v2_special_zone_transition(),
       upperSpecial:e.slot_v2_upper_special(),chainZone:e.slot_v2_chain_zone(),
       stockRestart:e.slot_v2_at_stock_restart(),upperComeback:e.slot_v2_upper_comeback(),
       revival:e.slot_v2_revival_state(),revivalFinalize:e.slot_v2_revival_finalize(),
       bonusTransition:e.slot_v2_bonus_transition(),entryTransition:e.slot_v2_entry_gate_transition(),
       atResolution:e.slot_v2_at_resolution(),atCycle:e.slot_v2_at_cycle(),
       atCold:e.slot_v2_at_cold(),czFinalize:e.slot_v2_cz_finalize(),
       bonusCycle:e.slot_v2_bonus_cycle(),normalCeiling:e.slot_v2_normal_ceiling_transition(),
       latentCompletion:e.slot_v2_latent_completion(),normalATTrigger:e.slot_v2_normal_at_trigger(),
       atSingleTransition:e.slot_v2_at_single_transition(),atWindow:e.slot_v2_at_window(),
       lowerFall:e.slot_v2_lower_fall_challenge(),lowerFallOutcome:e.slot_v2_lower_fall_push_outcome()};
  }
  function refresh(note) {
    if(!e)return;
    const s=snapshot(),stage=s.latent&255,left=(s.latent>>>8)&255,total=(s.latent>>>16)&255,route=s.latent>>>24;
    const mode=["通常A","通常B","天国","超天国","特殊"];
    const areaName=["NORMAL","CZ","BONUS","AT","復活"];
    $("mode").textContent=mode[e.slot_v2_normal_mode()]||"---";
    $("tier").textContent=s.at?["下位","中位","上位"][s.tier]:areaName[s.area];
    $("table").textContent=s.at?["通常","天国","超天国","特化"][e.slot_v2_at_table()]:"---";
    $("gameCount").textContent=totalSpins.toLocaleString("ja-JP");
    $("atLeft").textContent=s.at?s.left+"G":"0G";
    $("stocks").textContent=s.stock;
    const routeInfo=e.slot_v2_normal_route();
    $("normalPattern").textContent="P"+(routeInfo&255);
    $("ceiling").textContent="天井 "+((routeInfo>>>8)&65535)+"G";
    $("atPattern").textContent=s.at?"T"+e.slot_v2_at_table():"-";
    $("netRate").textContent=s.at?"AT TEST / C++ INTERNAL":"V2 C++抽選";
    const curr=Number(e.slot_v2_section_diff());
    $("diffCurrent").textContent=fmt(curr);
    $("diffRange").textContent="MAX "+fmt(Math.max(...diff))+" / MIN "+fmt(Math.min(...diff));
    $("diffGames").textContent=totalSpins+"G";
    $("totalDiff").textContent=fmt(curr);
    let scene="day",caption="昼ステージ";
    if(s.area===0){
      if(e.slot_v2_normal_mode()===0){scene=((e.slot_v2_normal_route()&255)%2)?"morning":"day";caption=scene==="morning"?"朝ステージ":"昼ステージ"}
      else if(e.slot_v2_normal_mode()===1){scene="evening";caption="夕方ステージ"}
      else if(e.slot_v2_normal_mode()===2){scene="night";caption="夜ステージ"}
      else if(e.slot_v2_normal_mode()===3){scene="deep-night";caption="深夜ステージ"}
      else {scene="special";caption="特殊ステージ"}
      if(stage===1)caption="何かが近づいている…";
    }else if(s.area===1){scene="challenge";caption="CHANCE ZONE"}
    else if(s.area===2){const kind=(s.bonus>>>8)&255;scene=kind===1?"episode":"bonus";caption=kind===1?"EPISODE BONUS":"BONUS"}
    else if(s.area===3){
      if(s.upperSpecial&1){scene="special";caption="上位特化ZONE"}
      else if(s.specialZone&1){scene="special";caption="特化ZONE"}
      else if(s.chainZone&1){scene="special";caption="CHAIN ZONE"}
      else {scene=s.tier===2?"upper":"at";caption=s.tier===2?"上位AT":"AT"}
    }else if(s.area===4){scene="challenge";caption="復活チャレンジ"}
    $("stageScreen").dataset.scene=scene;$("stageCaption").textContent=caption;
    $("statusLamp").className="status-lamp "+(s.area===3?"at":engineReady?"live":"");
    $("statusText").textContent=s.area===1?"CZ":s.area===2?"BONUS":s.area===3?"AT":"SETTING 6";
    $("lowerFallPush").hidden=(e.slot_v2_lower_fall_challenge()&255)!==3;
    if(note)say(note.title,note.detail,note.cls||"");
    $("debugState").textContent=JSON.stringify({
      build:"C++ 95/4/1 WASM V2",setting:e.slot_v2_setting(),
      area:areaName[s.area],phase:s.phase,spins:totalSpins,credit:credits,
      role:currentSpecial===3?"フリーズ":(((s.gate&1)&&(s.gate&2)&&s.phase===1)?"開始図柄":(ROLE[currentRole]||"?")),
      freezeActive:Boolean(e.slot_v2_freeze_active()),
      normalActual:e.slot_v2_normal_actual_games(),
      stage:stage===1?"潜伏":stage===2?"CZ経由":"なし",
      latentRoute:["通常イベント","通常予兆","予兆→CZ"][route],
      latentGamesLeft:left,latentTotal:total,
      gateActive:Boolean(s.gate&1),gateKind:(s.gate&2)?"非表示（停止結果待ち）":((s.gate>>>8)&255),
      pending:"0x"+e.slot_v2_pending_events().toString(16),lastPayout:lastWin,
      sectionDiff:curr
    },null,2);
    const canStart=s.phase===0||s.phase===3;
    $("lever").disabled=presentationLock||!engineReady||!canStart||!stopped.every(Boolean)||!$("lowerFallPush").hidden;
    document.querySelectorAll(".stop").forEach((b,i)=>{
      b.disabled=presentationLock||!engineReady||s.phase!==1||stopped[i];
      b.classList.toggle("active",!b.disabled);
      b.classList.remove("nav-active","nav-first");
      b.querySelector(".nav-order").textContent="—";
    });
    const nav=u32(e.slot_v2_bell_navigation());
    if(nav&1){
      const orders=[[0,1,2],[0,2,1],[1,0,2],[1,2,0],[2,0,1],[2,1,0]];
      const order=orders[(nav>>>8)&255]||orders[0],names=["左","中","右"];
      order.forEach((reel,idx)=>{
        if(!stopped[reel]){
          const b=document.querySelector('.stop[data-stop="'+reel+'"]');
          b.classList.add("nav-active");if(idx===0)b.classList.add("nav-first");
          b.querySelector(".nav-order").textContent=String(idx+1);
        }
      });
      $("naviState").textContent="🔔 "+order.map(i=>names[i]).join("→");
    }else $("naviState").textContent="NAVI —";
    prev=s;drawGraph();
  }
  function nextPressed(reel){
    const gate=e.slot_v2_entry_gate();
    const kind=(gate>>>8)&255;
    if(gate&2) {
      if(reel===0)return 4;
      if(reel===1)return 8;
      if(reel===2)return kind===2?11:12;
    }

    // The web prototype does not read real button-timing coordinates, so choose
    // a deterministic simulated press timing that never creates a misleading
    // left-middle watermelon for unrelated roles.
    // Strong cherry is the middle-cherry role by definition in this machine.
    // This browser prototype has no true reel-timing input, so feed a stable
    // simulated press timing that yields left-middle 🍒 and keeps REPLAY out
    // of the middle reel's visible window.
    if(currentRole===7){
      const strongCherryPress=[14,8,0];
      return strongCherryPress[reel]??0;
    }
    // Strong chance must present the intended 🐧 / 🍒 / 🐧 shape rather than
    // the old 🍉 / 🍉 / ❄️-looking stop.
    if(currentRole===10){
      const strongChancePress=[17,9,2];
      return strongChancePress[reel]??0;
    }
    // One-medal explicitly anchors the left reel on BAR, not WATERMELON.
    if(currentRole===2&&reel===0)return 0;
    return 0;
  }
  function settle(areaAtStart){
    const acquisition=e.slot_v2_acquisition();
    const status=acquisition&255,role=(acquisition>>>8)&255,award=(acquisition>>>16)&65535;
    let payout=status===1?award:0;
    if(role===2)payout=Math.max(1,payout);
    // Explicit prototype settlement adapter. Probabilities, AT state and
    // bonus completion continue to come from the newly compiled native C++.
    if(areaAtStart===3){
      const tier=e.slot_v2_at_tier();
      const net=[6,6,9][tier]||6;
      payout=Math.max(payout,3+net);
    }
    if(areaAtStart===2){
      payout=Math.max(payout,13); // 3 BET + 10 experimental BONUS net
    }
    if(payout)e.slot_v2_test_payout(payout);
    if(areaAtStart===2)e.slot_v2_test_bonus_gain(10);
    credits+=payout;
    lastWin=payout;
    const current=Number(e.slot_v2_section_diff());diff.push(current);
    if(diff.length>1000)diff.shift();
    $("payoutResult").textContent=payout+"枚";
  }
  function playPresentationEvents(events){
    if(!events.length)return false;
    presentationLock=true;
    let index=0;
    const next=()=>{
      if(index>=events.length){
        hideMachineCinematic();
        presentationLock=false;
        refresh();
        scheduleAuto();
        return;
      }
      const ev=events[index++];
      if(ev.role)stageCue(ev.role,"result");
      if(ev.stageClass)stage().classList.add(...ev.stageClass.split(" "));
      if(ev.fx)effect().className="effect-layer "+ev.fx;
      if(ev.omen){
        $("omenText").textContent=ev.omen;
        $("omenText").className="omen-text show";
      }
      if(ev.eye||ev.title)showMachineCinematic(ev.eye||"",ev.title||"",ev.sub||"",ev.cls||"");
      say(ev.banner||ev.title||"EVENT",ev.note||ev.sub||"",ev.bannerCls||"");
      if(ev.log)log(ev.log);
      later(next,ev.duration||820);
    };
    next();
    return true;
  }

  function collectPresentationEvents(old,s){
    const q=[];
    const add=(ev)=>q.push(ev);
    const latentStage=s.latent&255,oldLatentStage=old.latent&255;

    // Section cut and section reward can coexist; both must be shown.
    if(s.sectionReward&1)add({eye:"SECTION",title:"+2400 CROSS",sub:"有利区間切替",cls:"gold",banner:"有利区間 CROSS",note:"次区間報酬を抽選",bannerCls:"hot",log:"有利区間 CROSS"});
    if(s.sectionTransition&1){
      if(s.sectionTransition&(1<<1))add({role:"tier_up",eye:"SECTION",title:"AT 昇格",sub:"有利区間報酬",cls:"gold judge-result",banner:"AT昇格",note:"有利区間ルーレット",bannerCls:"hot",log:"有利区間：TIER UP"});
      if(s.sectionTransition&(1<<2))add({eye:"SECTION",title:"特化ZONE",sub:"有利区間報酬",cls:"gold",banner:"特化ZONE",note:"有利区間ルーレット",bannerCls:"hot",log:"有利区間：特化ZONE"});
      if(s.sectionTransition&(1<<3))add({eye:"SECTION",title:"上位特化ZONE",sub:"有利区間報酬",cls:"freeze",banner:"上位特化ZONE",note:"有利区間ルーレット",bannerCls:"premium",log:"有利区間：上位特化"});
    }

    // AT internal event itself.
    const atEvent=(s.atResolution>>>8)&255;
    if(atEvent===1)add({stageClass:"omen-purple",fx:"purple",omen:"ざわ…",banner:"AT予兆",note:"当たりの気配",bannerCls:"hot",log:"AT予兆"});
    if(atEvent===2)add({eye:"AT",title:"転落",sub:"継続ジャッジへ",cls:"red",banner:"転落",note:"AT内部抽選結果",log:"AT転落"});
    if(atEvent===3)add({eye:"ADD",title:"G数上乗せ",sub:"AT継続",cls:"gold",stageClass:"omen-gold screen-shock",fx:"gold flash",banner:"G数上乗せ",note:"AT内部抽選結果",bannerCls:"hot",log:"G数上乗せ"});
    if(atEvent===4)add({eye:"SPECIAL",title:"特化ZONE",sub:"AT内部当選",cls:"gold",banner:"特化ZONE",note:"AT内部抽選結果",bannerCls:"hot",log:"特化ZONE当選"});
    if(atEvent===5)add({eye:"EPISODE",title:"EPISODE BONUS",sub:"AT内部当選",cls:"gold judge-result",banner:"エピソード",note:"AT内部抽選結果",bannerCls:"hot",log:"EPISODE当選"});
    if(atEvent===6)add({eye:"PREMIUM",title:"上位特化ZONE",sub:"AT内部当選",cls:"freeze",banner:"上位特化ZONE",note:"AT内部抽選結果",bannerCls:"premium",log:"上位特化当選"});
    if(atEvent===7)add({eye:"CHAIN",title:"CHAIN ZONE",sub:"AT内部当選",cls:"judge",banner:"CHAIN ZONE",note:"AT内部抽選結果",bannerCls:"hot",log:"CHAIN ZONE当選"});

    // State starts resulting from the AT event.
    if((s.specialZone&1)&&!(old.specialZone&1))add({eye:"SPECIAL",title:"特化ZONE START",sub:"上乗せ抽選開始",cls:"gold",banner:"特化ZONE",note:"上乗せ抽選開始",bannerCls:"hot",log:"特化ZONE開始"});
    if((s.upperSpecial&1)&&!(old.upperSpecial&1))add({eye:"PREMIUM",title:"上位特化ZONE START",sub:"継続抽選開始",cls:"freeze",banner:"上位特化ZONE",note:"上位AT専用特化",bannerCls:"premium",log:"上位特化ZONE開始"});
    if((s.chainZone&1)&&!(old.chainZone&1))add({eye:"CHAIN",title:"CHAIN ZONE START",sub:"5Gセット開始",cls:"judge",banner:"CHAIN ZONE",note:"連続BONUS抽選",bannerCls:"hot",log:"CHAIN ZONE開始"});

    const szOutcome=s.specialZoneTransition&255;
    const szAdded=(s.specialZoneTransition>>>8)&65535;
    if(szOutcome===1)add({eye:"ADD",title:"+"+szAdded+"G",sub:"特化ZONE上乗せ",cls:"gold judge-result",banner:"G数上乗せ",note:"+"+szAdded+"G",bannerCls:"hot",log:"特化 +"+szAdded+"G"});
    if(szOutcome===2)add({eye:"HIT",title:"BONUS 予兆",sub:"特化ZONEから当選",cls:"judge",banner:"BONUS予兆",note:"開始図柄待ち",bannerCls:"hot",log:"特化→BONUS予兆"});

    if(s.upperSpecial&2){
      const added=(s.upperSpecial>>>8)&65535;
      if(s.upperSpecial&4)add({eye:"PREMIUM",title:"+"+added+"G",sub:"上位特化 継続",cls:"freeze",banner:"上位特化 継続",note:"+"+added+"G",bannerCls:"premium",log:"上位特化 +"+added+"G"});
      if(s.upperSpecial&8)add({eye:"END",title:"上位特化 END",sub:"ATへ復帰",cls:"gold",banner:"上位特化終了",note:"ATへ復帰",log:"上位特化終了"});
    }

    if(s.chainZone&2){
      const earned=(s.chainZone>>>16)&65535;
      if(s.chainZone&4)add({eye:"SIGNAL",title:"継続シグナル",sub:"CHAIN ZONE",cls:"judge",banner:"CHAIN SIGNAL",note:"継続抽選ヒット",bannerCls:"hot",log:"CHAIN SIGNAL"});
      if(s.chainZone&8)add({eye:"CONTINUE",title:"CHAIN 継続",sub:earned?("BONUS +"+earned):"次セットへ",cls:"gold",banner:"CHAIN継続",note:earned?("BONUS "+earned+"個"):"次セットへ",bannerCls:"hot",log:"CHAIN継続"});
      if(s.chainZone&16)add({eye:"END",title:"CHAIN END",sub:earned?("BONUS "+earned+"個獲得"):"ATへ復帰",cls:"red",banner:"CHAIN終了",note:earned?("BONUS "+earned+"個"):"ATへ復帰",log:"CHAIN終了"});
    }

    // Stock / restart.
    if(s.stock>old.stock)add({eye:"STOCK",title:"STOCK +"+(s.stock-old.stock),sub:"残り "+s.stock,cls:"gold",banner:"STOCK獲得",note:"ストック "+s.stock,bannerCls:"hot",log:"STOCK +"+(s.stock-old.stock)});
    if(s.stockRestart&1){
      const g=(s.stockRestart>>>8)&65535;
      add({eye:"STOCK",title:"AT RESTART",sub:g+"G",cls:"gold",banner:"ストック発動",note:g+"Gで再開",bannerCls:"hot",log:"STOCK RESTART "+g+"G"});
    }

    // A newly queued award is visible only as "hit / start-symbol wait".
    // Its AT/BONUS destination stays hidden until the actual entry symbols stop.
    if((s.gate&1)&&!(old.gate&1)&&!(s.gate&2)){
      add({eye:"HIT",title:"当たり",sub:"開始図柄待ち",cls:"judge",banner:"当たり",note:"次ゲーム以降で開始図柄を狙え",bannerCls:"hot",log:"当たり：開始図柄待ち"});
    }

    // Entry-symbol result is the point where the hidden destination becomes public.
    const entryOutcome=s.entryTransition&255;
    const entryStock=s.entryTransition>>>8;
    if(entryOutcome===1)add({role:"hit",eye:"START",title:"BONUS START",sub:"🟥7 🟥7 BAR",cls:"red judge-result",banner:"BONUS突入",note:"開始図柄成立",bannerCls:"hot",log:"BONUS START"});
    if(entryOutcome===2)add({role:"at",eye:"START",title:"AT START",sub:entryStock?("🟥777 / STOCK +"+entryStock):"🟥7 🟥7 🟥7",cls:"gold judge-result",banner:"AT突入",note:"開始図柄成立",bannerCls:"hot",log:"AT START"});

    // Direct special entries bypass entry_gate_transition.
    if(s.area===3&&old.area!==3&&entryOutcome!==2){
      const tier=["下位AT","中位AT","上位AT"][s.tier]||"AT";
      add({role:"at",eye:"START",title:tier,sub:"ASSIST TIME",cls:"gold judge-result",banner:tier+"突入",note:"AT開始",bannerCls:"hot",log:tier+"突入"});
    }
    if(s.area===2&&old.area!==2&&entryOutcome!==1){
      const ep=((s.bonus>>>8)&255)===1;
      add({eye:ep?"EPISODE":"BONUS",title:ep?"EPISODE BONUS":"BONUS START",sub:ep?"80枚":"50枚",cls:ep?"gold judge-result":"red judge-result",banner:ep?"EPISODE BONUS":"BONUS突入",note:"BONUS開始",bannerCls:ep?"premium":"hot",log:ep?"EPISODE BONUS":"BONUS突入"});
    }

    // BONUS lifecycle.
    const bonusOutcome=(s.bonusCycle>>>16)&255;
    if(bonusOutcome===1)add({eye:"BONUS",title:"BONUS COMPLETE",sub:"規定枚数到達",cls:"red",banner:"BONUS終了",note:"復帰判定へ",log:"BONUS COMPLETE"});
    if(bonusOutcome===2)add({eye:"EPISODE",title:"EPISODE 昇格",sub:"1%昇格成立",cls:"gold judge-result",banner:"EPISODE昇格",note:"Episode BONUSへ",bannerCls:"premium",log:"EPISODE昇格"});
    const bonusTrans=s.bonusTransition&255;
    if(bonusTrans===1)add({eye:"EPISODE",title:"EPISODE BONUS",sub:"昇格開始",cls:"gold judge-result",banner:"EPISODE BONUS",note:"80枚",bannerCls:"premium",log:"EPISODE BONUS開始"});
    if(bonusTrans===2)add({eye:"RETURN",title:"BONUS END",sub:"元の状態へ復帰",cls:"red",banner:"BONUS終了",note:"遊技状態へ復帰",log:"BONUS終了"});

    // CZ lifecycle.
    const czOut=s.czFinalize&255;
    if(czOut===1)add({eye:"HIT",title:"CZ 成功",sub:"当たり確定",cls:"gold judge-result",banner:"CZ成功",note:"当たりへ",bannerCls:"hot",log:"CZ成功"});
    if(czOut===2)add({eye:"END",title:"CZ 失敗",sub:"通常時へ",cls:"red",banner:"CZ失敗",note:"通常時へ移行",log:"CZ失敗"});
    if(czOut===3)add({eye:"HIT",title:"3スルー救済",sub:"当たり確定",cls:"gold judge-result",banner:"CZ救済",note:"当たりへ",bannerCls:"hot",log:"CZ3スルー救済"});
    if(s.area===1&&old.area!==1)add({eye:"CHANCE",title:"CHANCE ZONE",sub:"10G勝負",cls:"judge",banner:"CZ突入",note:"10Gチャレンジ",bannerCls:"hot",log:"CZ突入"});

    // Normal high / shortening / latent reveal.
    const high=s.normalFlow;
    if(high&2)add({eye:"STATE",title:"高確率 移行",sub:"抽選状態アップ",cls:"red",banner:"高確率 移行",note:"高確状態へ",bannerCls:"hot",log:"高確率 移行"});
    if(high&4)add({eye:"STATE",title:"高確率 終了",sub:"通常状態へ",cls:"",banner:"高確率 終了",note:"通常状態へ",log:"高確率 終了"});
    const shorten=(high>>>8)&65535;
    if(shorten)add({eye:"SHORTEN",title:"G数短縮",sub:shorten+"G",cls:"gold",banner:"G数短縮",note:shorten+"G短縮",bannerCls:"hot",log:"G数短縮 "+shorten+"G"});
    if(s.atCold&&!old.atCold)add({eye:"STATE",title:"冷遇",sub:"AT内部状態",cls:"red",banner:"冷遇",note:"成長領域を抑制",log:"冷遇状態"});
    if((s.atOmen&1)&&!(old.atOmen&1)&&atEvent!==1)add({stageClass:"omen-purple",fx:"purple",omen:"ざわ…",eye:"OMEN",title:"AT 予兆",sub:"当たりの気配",cls:"judge",banner:"AT予兆",note:"当たりの気配",bannerCls:"hot",log:"AT予兆"});
    if(s.latentCompletion===3)add({stageClass:"omen-red screen-shock",fx:"red flash",omen:"好機",eye:"EVENT",title:"当たり",sub:"通常イベントから告知",cls:"red judge-result",banner:"当たり",note:"開始図柄待ち",bannerCls:"hot",log:"通常イベント当たり"});
    if(s.latentCompletion===1)add({eye:"HIT",title:"当たり",sub:"予兆完了",cls:"judge",banner:"当たり",note:"開始図柄待ち",bannerCls:"hot",log:"予兆→当たり"});

    // Lower-tier predetermined fall challenge.
    const lowPhase=s.lowerFall&255,oldLow=old.lowerFall&255;
    if(lowPhase===2&&oldLow!==2)add({eye:"JUDGE",title:"継続チャレンジ",sub:"結果は内部で確定済み",cls:"judge",banner:"継続チャレンジ",note:"判定ベルを待て",bannerCls:"hot",log:"下位AT 継続チャレンジ"});
    if(lowPhase===3&&oldLow!==3)add({eye:"PUSH",title:"一撃 PUSH",sub:"ボタンを押せ",cls:"judge target",banner:"一撃PUSH",note:"継続ジャッジ",bannerCls:"hot",log:"一撃PUSH待ち"});

    // Upper comeback / general revival / AT end.
    if((s.upperComeback&1)&&!(old.upperComeback&1))add({eye:"COMEBACK",title:"64G 引き戻し",sub:"上位AT復帰を狙え",cls:"judge",banner:"上位引き戻し",note:"64Gチャレンジ",bannerCls:"hot",log:"上位引き戻し開始"});
    if((s.upperComeback&4)&&!(old.upperComeback&4))add({eye:"COMEBACK",title:"上位引き戻し",sub:"ジャッジ成功",cls:"gold judge-result",banner:"上位引き戻し",note:"上位AT開始図柄へ",bannerCls:"hot",log:"上位引き戻し成功"});
    if(s.area===4&&old.area!==4)add({eye:"REVIVAL",title:"復活チャレンジ",sub:"5G以内に復活を狙え",cls:"judge",banner:"復活チャレンジ",note:"5G勝負",bannerCls:"hot",log:"復活チャレンジ"});
    const rev=s.revivalFinalize&255;
    if(rev===1)add({eye:"REVIVAL",title:"復活！",sub:"ATへ復帰",cls:"gold judge-result",banner:"復活！",note:"終了ATを復活",bannerCls:"hot",log:"復活成功"});
    if(rev===2)add({eye:"END",title:"復活失敗",sub:"通常時へ",cls:"red",banner:"復活失敗",note:"通常時へ移行",log:"復活失敗"});
    if(old.area===3&&s.area!==3&&s.area!==4&&(s.upperComeback&1)===0)add({eye:"END",title:"AT END",sub:"通常時へ",cls:"red",banner:"AT終了",note:"通常時へ移行",log:"AT END"});

    // If no internal event consumed the result presentation, show physical role.
    if(!q.length)add({role:ROLE_KEY[currentRole]||"miss",banner:ROLE[currentRole]||"GAME",note:"リールの結果を確認",duration:500});
    return q;
  }

  function updateEvent(old,s){
    const events=collectPresentationEvents(old,s);
    return playPresentationEvents(events);
  }

  function finish(){
    const previous=startedSnapshot||prev;
    settle(startedArea);
    totalSpins++;
    stopped.fill(true);
    const s=snapshot();
    if(currentSpecial===3) {
      playPresentationEvents([{
        role:"freeze",eye:"PREMIUM",title:"FREEZE 確定",sub:"🟦7 🟦7 🟦7",
        cls:"freeze confirmed",banner:"FREEZE",note:"上位AT＋STOCK",
        bannerCls:"premium",log:"FREEZE：🟦777 / 上位AT",duration:1100
      }]);
    } else {
      updateEvent(previous,s);
    }
    if(totalSpins%5===0||s.area!==previous.area||s.latent!==previous.latent)log("成立役："+(ROLE[currentRole]||"?")+" / "+lastWin+"枚");
    refresh();
    scheduleAuto();
  }
  function stopOne(reel) {
    if(!engineReady||stopped[reel]||e.slot_v2_phase()!==1)return;
    const target=nextPressed(reel);
    let value=0,accepted=false;
    // The C++ engine remains the sole source of legal stop positions.
    // Probe alternate human button timings if a chosen stop is rejected.
    for(let i=0;i<21;i++){
      value=u32(e.slot_v2_stop(reel,(target+i)%21));
      const status=(value>>>16)&255;
      const center=value&255;

      // StrongCherry is defined by the visible stop itself:
      // left = middle-line CHERRY, middle = not middle-line REPLAY.
      // This browser prototype has no real physical button timing, so do not
      // settle on RoleMissed for this role; keep probing C++-legal timings.
      if(currentRole===7){
        if(reel===0){
          if(status===0&&u32(e.slot_v2_visible_symbol(0,center,0))===6){accepted=true;break;}
          continue;
        }
        if(reel===1){
          if(status===0&&u32(e.slot_v2_visible_symbol(1,center,0))!==5){accepted=true;break;}
          continue;
        }
      }

      if(status===0||status===5||status===6||status===7){accepted=true;break;}
    }
    if(!accepted){
      $("flagStatus").textContent="停止候補を確定できませんでした。状態："+e.slot_v2_phase();
      refresh();auto=false;return;
    }
    stopped[reel]=true;
    centers[reel]=value&255;
    stopVisualSpin(reel);
    reelVisualPositions[reel]=centers[reel];
    reelRender(reel,centers[reel]);
    refresh();
    if(stopped.every(Boolean))finish();
    else scheduleAuto();
  }
  function lever(){
    if(!engineReady||!stopped.every(Boolean))return;
    if((e.slot_v2_lower_fall_challenge()&255)===3)return;
    const old=snapshot();
    const result=u32(e.slot_v2_lever()),status=(result>>>24)&255;
    if(status!==0){say("内部処理待ち","PUSHや状態確定が必要（状態コード "+status+"）");auto=false;setAutoLabel();refresh();return;}
    startedArea=old.area;
    startedSnapshot=old;
    currentRole=result&255;
    currentSpecial=(result>>>8)&255;
    const gate=u32(e.slot_v2_entry_gate());
    const entryJudge=Boolean((gate&1)&&(gate&2));
    credits-=3;
    e.slot_v2_test_bet(3);
    // Do not leak the internally fixed AT/BONUS destination before the third reel reveals it.
    $("roleResult").textContent=currentSpecial===3?"フリーズ":(entryJudge?"開始図柄":(ROLE[currentRole]||"?"));
    $("payoutResult").textContent="判定中";

    if(currentSpecial===3){
      log("FREEZE発動");
      runFreezeEntrySequence();
      return;
    }
    if(e.slot_v2_phase()===2){
      runDirectSpecialSequence(old,currentSpecial);
      return;
    }

    stopped.fill(false);
    for(let i=0;i<3;i++) startVisualSpin(i);
    stageCue(entryJudge?"entry_judge":(ROLE_KEY[currentRole]||"miss"),"spin");

    if(entryJudge){
      runEntryGateCinematic();
      return;
    }

    say("回転中","STOPボタンでリールを止めよう");
    refresh();
    scheduleAuto();
  }
  function setAutoLabel(){
    $("autoToggle").textContent=auto?"AUTO ON":"AUTO OFF";
    $("autoToggle").classList.toggle("on",auto);
    $("autoToggle").setAttribute("aria-pressed",String(auto));
  }
  function scheduleAuto(){
    if(timer!==null){clearTimeout(timer);timer=null;}
    if(!auto||!engineReady||presentationLock)return;
    timer=setTimeout(()=>{
      timer=null;
      if(e.slot_v2_phase()===1){
        const next=u32(e.slot_v2_bell_navigation_next());
        const reel=next<3&&!stopped[next]?next:stopped.findIndex(v=>!v);
        if(reel>=0)stopOne(reel);
      }else if((e.slot_v2_lower_fall_challenge()&255)===3){
        auto=false;setAutoLabel();say("一撃PUSH待ち","50%の結果はC++内部で既に確定");
      }else if(stopped.every(Boolean))lever();
    },auto?115:200);
  }
  function reset(){
    if(timer!==null){clearTimeout(timer);timer=null;}
    auto=false;setAutoLabel();
    clearPresentationTimers();
    machine().classList.remove("freeze-all-off","freeze-return");
    hideMachineCinematic();clearStageClasses();
    stopAllVisualSpins();
    e.slot_v2_reset((Date.now()>>>0),((Date.now()/4294967296)>>>0));
    totalSpins=0;credits=1000;lastWin=0;currentRole=0;currentSpecial=0;
    stopped.fill(true);centers.fill(0);history.length=0;diff.splice(0,diff.length,0);
    startedSnapshot=null;
    $("history").replaceChildren();
    for(let i=0;i<3;i++){reelVisualPositions[i]=0;reelRender(i,0)}
    $("roleResult").textContent="---";$("payoutResult").textContent="0枚";
    say("READY","最新設定6・V2 C++ WASM");
    refresh();log("設定6リセット");
  }
  async function main(){
    for(const button of document.querySelectorAll(".stop")){
      button.addEventListener("click",()=>stopOne(Number(button.dataset.stop)));
    }
    $("lever").addEventListener("click",lever);
    $("autoToggle").addEventListener("click",()=>{auto=!auto;setAutoLabel();scheduleAuto()});
    $("reset").addEventListener("click",reset);
    $("lowerFallPush").addEventListener("click",()=>{
      const result=e.slot_v2_lower_fall_push();
      if(result===1){
        playPresentationEvents([{eye:"RESULT",title:"継続！",sub:"保持G数でAT継続",cls:"gold judge-result",banner:"継続！",note:"元のゲーム数を保持して続行",bannerCls:"hot",log:"下位AT 継続成功"}]);
      }else if(result===2){
        playPresentationEvents([{eye:"RESULT",title:"失敗",sub:"AT終了処理へ",cls:"red judge-result",banner:"継続失敗",note:"AT終了処理へ",log:"下位AT 継続失敗"}]);
      }else{
        say("PUSH未準備","判定ベルを待て");
      }
      refresh();scheduleAuto();
    });
    $("applyFlag").addEventListener("click",()=>{
      const choice=$("flagPicker").value;
      if(!choice){$("flagStatus").textContent="フラグを選択してください";return}
      const [ch,v]=choice.split(":").map(Number);
      const ok=e.slot_v2_debug_arm(ch,v)===1;
      $("flagStatus").textContent=ok?"C++に登録：次回の該当処理で適用":"現在の状態では設定不可。C++が拒否しました";
      if(ok)log("DEBUG "+($("flagPicker").selectedOptions[0]?.textContent||choice));
      refresh();
    });
    $("applyRoute").addEventListener("click",()=>{
      const v=$("routePicker").value;
      if(v===""){$("flagStatus").textContent="潜伏経路を選択してください";return}
      const ok=e.slot_v2_debug_arm(11,Number(v))===1;
      $("flagStatus").textContent=ok?"次の通常当選に潜伏経路を予約":"通常時の当選前に設定してください";
      refresh();
    });
    window.addEventListener("resize",drawGraph);
    try{
      // Absolute to this HTML directory, not the legacy slot.wasm.
      const response=await fetch("slot-v2.wasm?v=20260924-presentations8",{cache:"no-store"});
      if(!response.ok)throw Error("新WASM取得失敗: HTTP "+response.status);
      const binary=await response.arrayBuffer();
      if(!WebAssembly.validate(binary))throw Error("取得したV2 WASMが不正");
      const instance=await WebAssembly.instantiate(binary,{});
      e=instance.instance.exports;memory=e.memory;
      for(const key of ["slot_v2_reset","slot_v2_normal_latent","slot_v2_debug_arm",
        "slot_v2_debug_count","slot_v2_test_bet","slot_v2_test_payout","slot_v2_test_bonus_gain",
        "slot_v2_normal_flow","slot_v2_at_omen","slot_v2_special_zone_transition",
        "slot_v2_upper_special","slot_v2_chain_zone","slot_v2_at_stock_restart",
        "slot_v2_entry_gate_transition","slot_v2_at_cold",
        "slot_v2_cz_finalize","slot_v2_bonus_cycle","slot_v2_normal_ceiling_transition",
        "slot_v2_latent_completion","slot_v2_normal_at_trigger","slot_v2_at_single_transition",
        "slot_v2_at_window","slot_v2_lower_fall_challenge","slot_v2_lower_fall_push_outcome"]){
        if(typeof e[key]!=="function")throw Error("新C++ API不足: "+key);
      }
      if(e.slot_v2_debug_count()!==71)throw Error("デバッグ定義が71件ではありません");
      engineReady=true;populateFlags();
      $("flagStatus").textContent="最新のC++設定6 WASMから全71項目を取得済み";
      reset();
      $("eventNote").textContent="V2 C++ WASM＋演出システム読込済み。LEVERで開始";
    }catch(err){
      auto=false;setAutoLabel();say("読込エラー","旧WASMへ自動的に切り替えません");
      $("debugState").textContent=String(err&&err.stack||err);
      $("flagStatus").textContent="新WASMをロードできませんでした："+String(err&&err.message||err);
    }
  }
  main();
})();