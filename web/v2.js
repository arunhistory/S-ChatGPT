/* Latest native setting6 v2 browser adapter; never loads legacy slot.js. */
(() => {
  'use strict';
  const $ = id => document.getElementById(id);
  const ui = {
    lever:$('lever'), reset:$('reset'), auto:$('autoToggle'), push:$('lowerFallPush'),
    flag:$('debugFlag'), inject:$('injectFlag'), bonusTest:$('bonusTestGain'),
    debug:$('debugState'), history:$('history'), event:$('eventTitle'),
    note:$('eventNote'), banner:$('eventBanner'), stage:$('stageScreen'),
    caption:$('stageCaption'), role:$('roleResult'), payout:$('payoutResult'),
    navi:$('naviState'), mode:$('mode'), tier:$('tier'), table:$('table'),
    count:$('gameCount'), left:$('atLeft'), stocks:$('stocks'),
    pattern:$('normalPattern'), ceiling:$('ceiling'), atPattern:$('atPattern'),
    net:$('netRate'), diff:$('totalDiff'), diffNow:$('diffCurrent'),
    diffRange:$('diffRange'), diffGames:$('diffGames'),
    canvas:$('diffGraph'), status:$('statusText'), lamp:$('statusLamp')
  };
  const reels = [0,1,2].map(i => $('reel'+(i+1)));
  const buttons = [...document.querySelectorAll('.stop')];
  const pict = [
    ['snow','❄️',''],
    ['seven','7','RED'],
    ['alt-seven','7','BLUE'],
    ['bar','BAR',''],
    ['bell','🔔','BELL'],
    ['replay','↻','REPLAY'],
    ['cherry','🍒','CHERRY'],
    ['watermelon','🍉',''],
    ['penguin','🐧',''],
    ['snow','❄️','']
  ];
  const roleLabels = ['無役','ハズレ','1枚役','9枚ベル','15枚ベル','リプレイ',
    '弱チェリー','強チェリー','スイカ','弱チャンス目','強チャンス目',
    'ペンギンチャンス','赤7 AT開始','赤7・BAR BONUS開始'];
  const modeLabels = ['通常A','通常B','天国','超天国','特殊'];
  const areaLabels = ['通常時','CZ','BONUS','AT','復活チャレンジ'];
  const tierLabels = ['下位AT','中位AT','上位AT'];
  const tableLabels = ['通常','天国','超天国','特化'];
  const groupNames = ['','成立小役','直撃・フリーズ','通常基礎当選',
    '高確当選','小役による当選','天井','AT内部','CZ判定',
    '内部イベント（上級）','通常モード','予兆経路'];
  const readable = {
    None:'なし',Miss:'ハズレ',OneMedal:'1枚役',Bell9:'9枚ベル',
    Bell15:'15枚ベル',Replay:'リプレイ',WeakCherry:'弱チェリー',
    StrongCherry:'強チェリー',Watermelon:'スイカ',WeakChance:'弱チャンス目',
    StrongChance:'強チャンス目',PenguinChance:'ペンギンチャンス',
    EntryAT:'赤7 AT開始役',EntryBonus:'赤7＋BAR BONUS開始役',
    MiddleATStock:'中位AT＋ストック直撃',UpperAT:'上位AT直撃',
    Freeze:'フリーズ',Bonus:'通常当たり',LowerAT:'下位AT',MiddleAT:'中位AT',
    CZ:'CZ',Hit:'当選',Fall:'転落',AddGames:'上乗せ',Special:'特化',
    Episode:'エピソード',UpperSpecial:'上位特化',ChainZone:'連鎖ゾーン',
    ThirdMiss:'CZ3スルー',NormalHit:'通常当たり',
    NormalA:'通常A',NormalB:'通常B',Heaven:'天国',SuperHeaven:'超天国',
    NormalEvent:'通常イベント告知（1％）',Omen:'通常予兆（4％）',
    OmenCZ:'予兆→CZ（95％）'
  };
  let e=null, totalGames=0, running=false, auto=false, interval=null, timer=null;
  let spins=[0,0,0], stopped=[true,true,true], current=null;
  let lastDiff=0, rangeMin=0, rangeMax=0;
  let points=[{x:0,y:0}], currentFlag='';
  const seed=() => {
    const t=Date.now(), a=(Math.random()*4294967296)>>>0;
    return [(t>>>0)^a, (Math.floor(t/4294967296)>>>0)^((a<<5)>>>0)];
  };
  function label(text,note='') {
    ui.event.textContent=text;
    ui.note.textContent=note;
  }
  function log(s) {
    const li=document.createElement('li');
    const b=document.createElement('b');
    b.textContent=String(totalGames)+'G';
    const span=document.createElement('span');
    span.textContent=s; li.append(b,span);
    ui.history.prepend(li);
    while(ui.history.children.length>36) ui.history.lastElementChild.remove();
  }
  const q = (name,...args) => e['slot_v2_'+name](...args);
  function symbol(code){
    const info=pict[code]||pict[0];
    return '<span data-kind="'+info[0]+'">'+info[1]+
      (info[2]?'<small>'+info[2]+'</small>':'')+'</span>';
  }
  function paintReel(i,center) {
    let out='';
    for(let off=-1;off<=1;off++) out+=symbol(q('visible_symbol',i,center,off));
    reels[i].innerHTML=out;
  }
  function draw() {
    const canvas=ui.canvas, rect=canvas.getBoundingClientRect();
    const scale=Math.max(1,window.devicePixelRatio||1);
    const w=Math.max(1,rect.width),h=Math.max(1,rect.height);
    canvas.width=Math.round(w*scale);canvas.height=Math.round(h*scale);
    const ctx=canvas.getContext('2d');ctx.setTransform(scale,0,0,scale,0,0);
    ctx.clearRect(0,0,w,h);
    const low=Math.min(0,...points.map(p=>p.y)),high=Math.max(0,...points.map(p=>p.y));
    const spread=Math.max(100,high-low),lo=low-spread*.13,hi=high+spread*.13;
    const first=points[0].x,end=Math.max(first+1,points[points.length-1].x);
    const px=x=>5+(x-first)/(end-first)*Math.max(1,w-10);
    const py=y=>5+(hi-y)/(hi-lo)*Math.max(1,h-10);
    ctx.strokeStyle='#6e81964d';ctx.lineWidth=1;
    for(let i=1;i<4;i++){const y=h*i/4;ctx.beginPath();ctx.moveTo(0,y);ctx.lineTo(w,y);ctx.stroke();}
    ctx.strokeStyle='#aac7e5';ctx.beginPath();ctx.moveTo(0,py(0));ctx.lineTo(w,py(0));ctx.stroke();
    ctx.strokeStyle='#7ed9ff';ctx.lineWidth=2;ctx.beginPath();
    points.forEach((p,i)=>{if(i===0)ctx.moveTo(px(p.x),py(p.y));else ctx.lineTo(px(p.x),py(p.y));});
    ctx.stroke();
    ui.diffRange.textContent='MAX '+(rangeMax>0?'+':'')+rangeMax+' / MIN '+rangeMin;
    ui.diffGames.textContent=totalGames+'G';
  }
  function updateDiff(){
    const diff=Number(q('total_diff'));
    ui.diff.textContent=(diff>0?'+':'')+diff;
    ui.diffNow.textContent=(diff>0?'+':'')+diff;
    if(lastDiff!==diff||points[points.length-1].x!==totalGames) {
      lastDiff=diff;rangeMin=Math.min(rangeMin,diff);rangeMax=Math.max(rangeMax,diff);
      points.push({x:totalGames,y:diff});
      if(points.length>700) points=[points[0],...points.filter((_,i)=>i%2===0).slice(1)];
    }
    draw();
  }
  function readStage(){
    const area=q('machine_area');
    const latent=q('normal_latent')>>>0, phase=q('lower_fall_challenge');
    const latentStage=latent&255, left=(latent>>>8)&255;
    const gate=q('entry_gate')>>>0;
    let title=areaLabels[area]||'通常時',scene='normal',note='';
    if(latentStage===1) {
      title='予兆・潜伏',scene='omen';
      note='内部確定済み（告知まで '+left+'G以内）';
    } else if(latentStage===2) {
      title='CZ：確定当選を保持',scene='cz';
      note='成立済みのボーナス／ATは消滅しない';
    } else if(area===1) {
      title='CZ',scene='cz';note='C++役連動CZ';
    } else if(area===2) {
      title='BONUS',scene='bonus';note='残り '+((q('bonus_state')>>>16)&65535)+'枚（C++管理）';
    } else if(area===3) {
      title=tierLabels[q('at_tier')]||'AT',scene='at';
      note='ST残り '+q('at_games_left')+'G';
    } else if(area===4) {
      title='復活チャレンジ',scene='revival';
      note='残り '+((q('revival_state')>>>8)&255)+'G';
    } else if(gate&1) {
      title=((gate>>>8)&255)===2?'AT開始図柄待機':'BONUS開始図柄待機';
      note='権利確定済み。赤7の入賞を待機';
      scene='omen';
    }
    if((phase&255)===3){title='一撃復活 PUSH';note='成功可否は既にC++内部で確定';scene='revival';}
    ui.caption.textContent=title+' '+note;
    ui.stage.dataset.scene=scene;
    return {title,note,area,gate,latentStage,latent,fall:phase&255};
  }
  function status(){
    const st=readStage();
    ui.mode.textContent=modeLabels[q('normal_mode')]||'—';
    ui.tier.textContent=q('at_active')?(tierLabels[q('at_tier')]||'AT'):'—';
    ui.table.textContent=q('at_active')?(tableLabels[q('at_table')]||'—'):'—';
    ui.count.textContent=totalGames;
    ui.left.textContent=q('at_active')?q('at_games_left')+'G':'—';
    ui.stocks.textContent=q('stock_count');
    const nr=q('normal_route')>>>0;
    ui.pattern.textContent='P'+(nr&255);
    ui.ceiling.textContent='天井 '+((nr>>>8)&65535)+'G';
    ui.atPattern.textContent=st.title;
    ui.net.textContent='役払い出しの計測値';
    ui.status.textContent=st.area===3?'AT':st.area===2?'BONUS':st.area===1?'CZ':'C++ v2 READY';
    ui.lamp.classList.toggle('live',st.area!==3);
    ui.lamp.classList.toggle('at',st.area===3);
    ui.push.hidden=st.fall!==3;
    ui.bonusTest.hidden=st.area!==2;
    ui.lever.disabled=!e||running||st.fall===3;
    buttons.forEach((b,i)=>{b.disabled=!running||stopped[i];b.classList.toggle('active',running&&!stopped[i]);});
    const next=q('bell_navigation_next')>>>0;
    const nav=(q('bell_navigation')&1)!==0;
    const order=(q('bell_navigation')>>>8)&255;
    const orders=[[0,1,2],[0,2,1],[1,0,2],[1,2,0],[2,0,1],[2,1,0]];
    const arr=orders[order]||orders[0];
    buttons.forEach((b,i)=>{
      const n=b.querySelector('.nav-order');
      n.textContent=nav?String(arr.indexOf(i)+1):'—';
      b.classList.toggle('nav-active',nav&&next===i&&!stopped[i]);
    });
    ui.navi.textContent=nav?(next<3?'NAVI '+['左','中','右'][next]+' FIRST':'NAVI 判定済'):'NAVI —';
    ui.debug.textContent=JSON.stringify({
      engine:'slot-v2.wasm (native C++)',setting:q('setting'),
      area:st.title,phase:q('phase'),normalActualGames:q('normal_actual_games'),
      latencyPacked:st.latent,entryGate:st.gate,stock:q('stock_count'),
      atGamesLeft:q('at_games_left'),bonusState:q('bonus_state'),
      pendingEvents:q('pending_events')>>>0,flagPending:currentFlag||'通常抽選',
      sectionDiff:String(q('section_diff')),totalDiff:String(q('total_diff'))
    },null,2);
    updateDiff();
  }
  function pauseMotion(){
    if(interval!==null)clearInterval(interval);
    interval=null;
    reels.forEach(r=>r.classList.remove('spinning'));
  }
  function motion(){
    pauseMotion();
    interval=setInterval(()=>{
      if(!running)return;
      for(let i=0;i<3;i++)if(!stopped[i]){
        spins[i]=(spins[i]+1)%21;paintReel(i,spins[i]);
      }
    },92);
    reels.forEach((r,i)=>r.classList.toggle('spinning',!stopped[i]));
  }
  function advanceBonusIfNeeded(payout){
    if(!current||!current.wasBonus)return;
    // C++ bonus progression expects actual measured net gain.
    q('bonus_net_gain',payout-3);
  }
  function complete(){
    pauseMotion();running=false;
    const ac=q('acquisition')>>>0;
    const payout=(ac>>>16)&65535;
    if(payout>0)q('payout',payout);
    advanceBonusIfNeeded(payout);
    ui.role.textContent=roleLabels[current.role]||'—';
    ui.payout.textContent=payout+'枚';
    const previous=current.before,now=readStage();
    if(previous.latentStage===1&&now.latentStage===2) {
      label('予兆終了 → CZ','内部確定済みの当選を維持');
      log('予兆を経由してCZへ');
    } else if(previous.latentStage===1&&now.latentStage===0&&(now.gate&1)){
      label('当選告知','潜伏終了。開始図柄を狙え');
      log('潜伏後、確定当選を告知');
    } else if(now.area===3&&previous.area!==3){
      label('AT START',tierLabels[q('at_tier')]||'AT');
      log('AT開始');
    } else if(now.area===2&&previous.area!==2){
      label('BONUS START','C++ v2');
      log('BONUS開始');
    } else if(previous.area===1&&now.gate&1){
      label('CZ成功','確定済みの当選へ移行');
      log('CZ成功／開始図柄待機');
    } else if(now.area===4&&previous.area!==4){
      label('復活チャレンジ','既存のC++復活判定へ');
      log('復活チャレンジ開始');
    } else if(now.fall===3){
      label('一撃PUSH','下位ATの復活判定結果を開示');
      log('復活PUSH待機');
    } else if(now.latentStage===1){
      label('通常演出', '予兆進行（開始図柄の抽選なし）');
    } else if(now.gate&1){
      label('開始図柄待機','赤7を狙え');
    } else if(now.area===1){
      label('CHANCE ZONE','内部CZ抽選中');
    } else {
      label(roleLabels[current.role]||'NORMAL','次のレバーへ');
    }
    current=null;status();planAuto();
  }
  function start(){
    if(!e||running)return;
    if((q('lower_fall_challenge')&255)===3){
      label('一撃PUSH待ち','PUSHで固定結果を開示');
      return;
    }
    const before=readStage();
    const wasBonus=(q('bonus_state')&1)!==0;
    const lever=q('lever')>>>0,statusCode=(lever>>>24)&255;
    if(statusCode!==0){
      label('レバー受付不可','内部遷移またはPUSH待ち。DEBUG STATEを確認');
      log('レバー拒否：C++ status '+statusCode);
      status();if(auto)setAuto(false);return;
    }
    q('bet',3);
    totalGames++;
    const role=lever&255,special=(lever>>>8)&255;
    current={before,wasBonus,role,special};
    currentFlag='';
    ui.role.textContent=roleLabels[role]||'特殊抽選';
    ui.payout.textContent='判定中';
    // Explicit direct-special phases have no physical three-reel stop.
    if(q('phase')===2){
      q('complete_special');current=null;
      label('SPECIAL DIRECT',special===1?'中位AT＋ストック':special===2?'上位AT直撃':'特殊当選');
      log('特殊直撃：C++内部処理');
      status();planAuto();return;
    }
    running=true;stopped=[false,false,false];
    spins=spins.map((n,i)=>(n+i+1)%21);
    motion();
    label(special===3?'FREEZE':before.gate&1?'START CHANCE':'レバーON',
      special===3?'青7逆転停止':before.gate&1?'当選権利は内部確定':'各リールを押して停止');
    status();planAuto();
  }
  function findCenter(reel,sym){
    for(let p=0;p<21;p++)if(q('symbol_at',reel,p)===sym)return p;
    return spins[reel];
  }
  function stop(reel,autoAim=false){
    if(!e||!running||stopped[reel])return;
    let pressed=spins[reel];
    const role=current.role;
    if(autoAim){
      if(role===12||role===13) pressed=findCenter(reel,role===13&&reel===2?3:1);
      else if((q('bell_navigation')&1) && (role===3||role===4))
        pressed=findCenter(reel,4);
      else if(role===5)pressed=findCenter(reel,5);
    }
    let result=q('stop',reel,pressed)>>>0;
    let code=(result>>>16)&255;
    if(autoAim && ![0,5,6,7].includes(code)){
      for(let p=0;p<21;p++){
        result=q('stop',reel,p)>>>0;
        code=(result>>>16)&255;
        if([0,5,6,7].includes(code))break;
      }
    }
    if(![0,5,6,7].includes(code)){
      label('停止位置が不成立','再度STOPを押してね（C++コード '+code+'）');
      status();return;
    }
    stopped[reel]=true;spins[reel]=q('stopped_position',reel)>>>0;
    paintReel(reel,spins[reel]);reels[reel].classList.remove('spinning');
    if(q('phase')===3){
      complete();
    }else{
      status();planAuto();
    }
  }
  function setAuto(next){
    auto=next;
    ui.auto.textContent='AUTO '+(auto?'ON':'OFF');
    ui.auto.classList.toggle('on',auto);
    ui.auto.setAttribute('aria-pressed',String(auto));
    if(timer!==null)clearTimeout(timer);timer=null;
    if(auto)planAuto();
  }
  function planAuto(){
    if(!auto||!e)return;
    if(timer!==null)clearTimeout(timer);
    timer=setTimeout(()=>{
      timer=null;
      if(!auto)return;
      if((q('lower_fall_challenge')&255)===3){
        setAuto(false);
        label('一撃PUSH','復活判定は手動で実行');
        return;
      }
      if(running){
        let reel=(q('bell_navigation_next')>>>0);
        if(reel>=3||stopped[reel])reel=stopped.indexOf(false);
        if(reel>=0)stop(reel,true);
      }else start();
    },running?250:420);
  }
  function makeFlags(){
    const groups={};
    const mem=new Uint8Array(e.memory.buffer);
    const decoder=new TextDecoder();
    const count=q('debug_count');
    for(let i=0;i<count;i++){
      const ch=q('debug_channel',i),value=q('debug_value',i),ptr=q('debug_name_ptr',i);
      if(!ptr||ch===0)continue;
      let end=ptr;
      while(end<mem.length&&mem[end]!==0)end++;
      const raw=decoder.decode(mem.subarray(ptr,end));
      const short=raw.split('::').pop();
      if(!groups[ch]){
        groups[ch]=document.createElement('optgroup');
        groups[ch].label=groupNames[ch]||'内部 '+ch;
        ui.flag.append(groups[ch]);
      }
      const opt=document.createElement('option');
      opt.value=ch+':'+value;
      opt.textContent=(readable[short]||short)+' · '+raw;
      groups[ch].append(opt);
    }
    if(count!==70)throw new Error('C++ debug catalogue mismatch: '+count);
  }
  function reset(){
    if(!e)return;
    setAuto(false);pauseMotion();running=false;current=null;currentFlag='';
    const [a,b]=seed();q('reset',a,b);
    totalGames=0;lastDiff=0;rangeMin=0;rangeMax=0;points=[{x:0,y:0}];
    stopped=[true,true,true];spins=[0,7,12];
    reels.forEach((_,i)=>paintReel(i,spins[i]));
    ui.history.replaceChildren();
    ui.role.textContent='—';ui.payout.textContent='0枚';
    label('C++ v2 READY','設定6・95% CZ経由 / 4%予兆 / 1%通常イベント');
    status();log('設定6初期化');
  }
  ui.lever.addEventListener('click',start);
  buttons.forEach((b,i)=>b.addEventListener('click',()=>stop(i,false)));
  ui.reset.addEventListener('click',reset);
  ui.auto.addEventListener('click',()=>setAuto(!auto));
  ui.push.addEventListener('click',()=>{
    if(!e||running)return;
    const outcome=q('lower_fall_push');
    label(outcome===1?'一撃成功':outcome===2?'一撃失敗':'判定不可',
      'C++が予め決定した結果');
    log(outcome===1?'一撃復活成功':outcome===2?'一撃復活失敗':'一撃PUSH不可');
    status();planAuto();
  });
  ui.inject.addEventListener('click',()=>{
    if(!e||!ui.flag.value)return;
    const [channel,value]=ui.flag.value.split(':').map(Number);
    const name=ui.flag.selectedOptions[0].textContent;
    const ok=q('debug_arm',channel,value)===1;
    if(ok){currentFlag=name;label('内部フラグ受付',name);log('DEBUG: '+name);}
    else{label('フラグ指定不可','現在のC++状態／フェーズでは設定できない');}
    status();
  });
  ui.bonusTest.addEventListener('click',()=>{
    if(!e||running||!(q('bonus_state')&1))return;
    q('bonus_net_gain',10);
    label('BONUS検証 +10純増','デバッグ専用。差枚会計には加算していない');
    log('DEBUG BONUS +10純増');status();
  });
  window.addEventListener('resize',()=>{if(e)draw();});
  (async()=>{
    try{
      const res=await fetch('slot-v2.wasm',{cache:'no-store'});
      if(!res.ok)throw new Error('WASM HTTP '+res.status);
      const bin=await res.arrayBuffer();
      if(!WebAssembly.validate(bin))throw new Error('不正なWASMファイル');
      const instantiated=await WebAssembly.instantiate(bin,{});
      e=instantiated.instance.exports;
      for(const key of ['reset','lever','stop','normal_latent','debug_count',
        'debug_name_ptr','debug_arm','bonus_net_gain','total_diff']){
        if(typeof e['slot_v2_'+key]!=='function')
          throw new Error('WASMに '+key+' がない。古いWASMが配信されています');
      }
      makeFlags();reset();
      log('最新C++ WASM読込成功：'+bin.byteLength+' bytes');
    }catch(err){
      pauseMotion();setAuto(false);
      label('WASM LOAD ERROR',String(err));
      ui.debug.textContent=String(err);
      ui.lever.disabled=true;ui.inject.disabled=true;
    }
  })();
})();
