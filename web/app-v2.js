(() => {
  "use strict";
  const $ = id => document.getElementById(id);
  let e = null, memory = null, engineReady = false, auto = false, timer = null;
  let totalSpins = 0, startedArea = 0, currentRole = 0, lastWin = 0, credits = 1000;
  const stopped = [true,true,true], centers = [0,0,0], history = [], diff = [0];
  const reelSpinTimers = [null,null,null];
  const reelVisualPositions = [0,7,14];
  let prev = { area:0, gate:0, latent:0 }, startedSnapshot = null;
  const ROLE = ["なし","ハズレ","1枚役","9枚ベル","15枚ベル","リプレイ",
     "弱チェリー","強チェリー","スイカ","弱チャンス目","強チャンス目",
     "ペンギンチャンス","AT開始7","BONUS開始7"];
  const SYM = ["?","7","7","BAR","🔔","↻","🍒","🍉","🐧","❄️"];
  const KIND = ["unknown","seven","alt-seven","bar","bell","replay","cherry","watermelon","penguin","miss"];
  const CHANNEL = {"1":"小役・開始図柄","2":"特殊直撃","3":"通常抽選","4":"高確抽選",
    "5":"レア役当選","6":"天井","7":"AT内部当選","8":"CZ結果",
    "9":"内部Pendingフラグ","10":"通常モード","11":"潜伏経路"};
  const TRANSLATE = {
    "RoleFlag::None":"なし","RoleFlag::Miss":"ハズレ","RoleFlag::OneMedal":"1枚役",
    "RoleFlag::Bell9":"9枚ベル","RoleFlag::Bell15":"15枚ベル",
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
    $("reel"+(i+1)).classList.remove("spinning");
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
       cz:e.slot_v2_cz_cycle(),phase:e.slot_v2_phase()};
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
    $("stageScreen").dataset.scene=s.area===1?"cz":s.area===2?"bonus":s.area===3?"at":"normal";
    $("stageCaption").textContent=s.area===1?"CHANCE ZONE":s.area===2?"BONUS":
        s.area===3?"ASSIST TIME":s.area===4?"復活チャレンジ":stage===1?"何かが近づいている…":"通常ステージ";
    $("statusLamp").className="status-lamp "+(s.area===3?"at":engineReady?"live":"");
    $("statusText").textContent=s.area===1?"CZ":s.area===2?"BONUS":s.area===3?"AT":"SETTING 6";
    $("lowerFallPush").hidden=(e.slot_v2_lower_fall_challenge()&255)!==3;
    if(note)say(note.title,note.detail,note.cls||"");
    $("debugState").textContent=JSON.stringify({
      build:"C++ 95/4/1 WASM V2",setting:e.slot_v2_setting(),
      area:areaName[s.area],phase:s.phase,spins:totalSpins,credit:credits,
      role:ROLE[currentRole],normalActual:e.slot_v2_normal_actual_games(),
      stage:stage===1?"潜伏":stage===2?"CZ経由":"なし",
      latentRoute:["通常イベント","通常予兆","予兆→CZ"][route],
      latentGamesLeft:left,latentTotal:total,
      gateActive:Boolean(s.gate&1),gateKind:(s.gate>>>8)&255,
      pending:"0x"+e.slot_v2_pending_events().toString(16),lastPayout:lastWin,
      sectionDiff:curr
    },null,2);
    const canStart=s.phase===0||s.phase===3;
    $("lever").disabled=!engineReady||!canStart||!stopped.every(Boolean)||!$("lowerFallPush").hidden;
    document.querySelectorAll(".stop").forEach((b,i)=>{
      b.disabled=!engineReady||s.phase!==1||stopped[i];
      b.classList.toggle("active",!b.disabled);
      b.querySelector(".nav-order").textContent="—";
    });
    const next=u32(e.slot_v2_bell_navigation_next());
    if(next<3 && !stopped[next])document.querySelector('.stop[data-stop="'+next+'"] .nav-order').textContent="①";
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
  function updateEvent(old,s){
    const stage=s.latent&255;
    const newGate=Boolean(s.gate&1),oldGate=Boolean(old.gate&1);
    if(s.area===3&&old.area!==3){say("AT突入","内部抽選で確定したATを開始！","hot");log("AT突入");return;}
    if(s.area===2&&old.area!==2){say("BONUS突入","C++ボーナス処理へ移行","hot");log("BONUS突入");return;}
    if(s.area===4&&old.area!==4){say("復活チャレンジ","5G以内に復活を狙え","hot");log("復活チャレンジ");return;}
    if(s.area===1&&old.area!==1){say("CZ突入","内部の当選権利はそのまま維持","hot");log("CZ突入");return;}
    if(newGate&&!oldGate){const type=((s.gate>>>8)&255)===2?"AT":"BONUS";say(type+"確定","リールで開始図柄を狙え","hot");log(type+"確定");return;}
    if(stage===1&&(old.latent&255)!==1){say("予兆開始","何かが起こるかも…","");log("予兆開始");return;}
    if(s.at&&((e.slot_v2_at_resolution()>>>8)&255)>0){const a=["","当たり","転落","G上乗せ","特化","エピソード","上位特化","チェーン"];const event=a[(e.slot_v2_at_resolution()>>>8)&255]||"ATイベント";say(event,"AT内部抽選結果","hot");log(event);return;}
    say(ROLE[currentRole]||"GAME","リールの結果を確認",""); 
  }
  function finish(){
    const previous=startedSnapshot||prev;
    settle(startedArea);
    totalSpins++;
    stopped.fill(true);
    const s=snapshot();
    updateEvent(previous,s);
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
    credits-=3;
    e.slot_v2_test_bet(3);
    $("roleResult").textContent=ROLE[currentRole]||"?";
    $("payoutResult").textContent="判定中";
    if(e.slot_v2_phase()===2){
      say("特殊抽選","直撃結果を内部確定中","premium");
      e.slot_v2_complete_special();
      // Special game has no ordinary reel stops. Accounting was applied
      // once at the accepted lever, as it is for regular games.
      stopped.fill(true);lastWin=0;totalSpins++;
      diff.push(Number(e.slot_v2_section_diff()));
      updateEvent(old,snapshot());refresh();scheduleAuto();return;
    }
    stopped.fill(false);
    for(let i=0;i<3;i++) startVisualSpin(i);
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
    if(!auto||!engineReady)return;
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
    stopAllVisualSpins();
    e.slot_v2_reset((Date.now()>>>0),((Date.now()/4294967296)>>>0));
    totalSpins=0;credits=1000;lastWin=0;currentRole=0;
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
      say(result===1?"継続！":result===2?"復活失敗":"PUSH未準備",
        result===1?"元のゲーム数を保持して続行":result===2?"内部終了処理へ":"");
      log("一撃PUSH："+result);
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
      const response=await fetch("slot-v2.wasm?v=20260923-v2a",{cache:"no-store"});
      if(!response.ok)throw Error("新WASM取得失敗: HTTP "+response.status);
      const binary=await response.arrayBuffer();
      if(!WebAssembly.validate(binary))throw Error("取得したV2 WASMが不正");
      const instance=await WebAssembly.instantiate(binary,{});
      e=instance.instance.exports;memory=e.memory;
      for(const key of ["slot_v2_reset","slot_v2_normal_latent","slot_v2_debug_arm",
        "slot_v2_debug_count","slot_v2_test_bet","slot_v2_test_payout","slot_v2_test_bonus_gain"]){
        if(typeof e[key]!=="function")throw Error("新C++ API不足: "+key);
      }
      if(e.slot_v2_debug_count()!==70)throw Error("デバッグ定義が70件ではありません");
      engineReady=true;populateFlags();
      $("flagStatus").textContent="最新のC++設定6 WASMから全70項目を取得済み";
      reset();
      $("eventNote").textContent="61KB台のV2 C++ WASM読込済み。LEVERで開始";
    }catch(err){
      auto=false;setAutoLabel();say("読込エラー","旧WASMへ自動的に切り替えません");
      $("debugState").textContent=String(err&&err.stack||err);
      $("flagStatus").textContent="新WASMをロードできませんでした："+String(err&&err.message||err);
    }
  }
  main();
})();