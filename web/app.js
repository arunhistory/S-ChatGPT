(async () => {
  const $ = (s) => document.querySelector(s);
  const els = {
    mode: $('#mode'), tier: $('#tier'), table: $('#table'), gameCount: $('#gameCount'),
    atLeft: $('#atLeft'), stocks: $('#stocks'),
    eventBanner: $('#eventBanner'), eventTitle: $('#eventTitle'), eventNote: $('#eventNote'),
    normalPattern: $('#normalPattern'), ceiling: $('#ceiling'), atPattern: $('#atPattern'),
    netRate: $('#netRate'), totalDiff: $('#totalDiff'), history: $('#history'),
    diffGraph: $('#diffGraph'), diffCurrent: $('#diffCurrent'), diffRange: $('#diffRange'), diffGames: $('#diffGames'),
    debug: $('#debugState'), statusLamp: $('#statusLamp'), statusText: $('#statusText'),
    machine: $('.machine'), machineCinematic: $('#machineCinematic'), cinematicEyebrow: $('#cinematicEyebrow'), cinematicTitle: $('#cinematicTitle'), cinematicSub: $('#cinematicSub'),
    lever: $('#lever'), autoToggle: $('#autoToggle'), reset: $('#reset'),
    roleResult: $('#roleResult'), payoutResult: $('#payoutResult'), roleTest: $('#roleTest'),
    stageScreen: $('#stageScreen'), characterSprite: $('#characterSprite'),
    naviState: $('#naviState'),
    effectLayer: $('#effectLayer'), cutinLayer: $('#cutinLayer'),
    cutinEyebrow: $('#cutinEyebrow'), cutinTitle: $('#cutinTitle'),
    cutinSub: $('#cutinSub'), stageCaption: $('#stageCaption')
  };

  const reels = [$('#reel1'), $('#reel2'), $('#reel3')];
  const reelArtTracks = [$('#reelArt1'), $('#reelArt2'), $('#reelArt3')];
  const stops = [...document.querySelectorAll('.stop')];
  const history = [];

  const symbols = [
    { kind:'seven', html:'7' },
    { kind:'alt-seven', html:'7<small>BLUE</small>' },
    { kind:'bell', html:'🔔<small>BELL</small>' },
    { kind:'cherry', html:'🍒<small>CHERRY</small>' },
    { kind:'watermelon', html:'🍉<small>WATERMELON</small>' },
    { kind:'penguin', html:'🐧<small>PENGUIN</small>' },
    { kind:'replay', html:'↻<small>REPLAY</small>' },
    { kind:'bar', html:'BAR' },
    { kind:'chance', html:'★<small>CHANCE</small>' },
    { kind:'miss', html:'×<small>ハズレ</small>' }
  ];
  const symbolByKind = Object.fromEntries(symbols.map(s => [s.kind, s]));

  // 21コマ配列を実機式停止制御に合わせて再設計中。
  // LEFT は確定。BARを狙えば隣接する🍉/🍒をフォローできる配置。
  // MIDDLE / RIGHT は次工程で再配置するため現行配列を一時維持。
  const reelStrips = [
    ['bar','cherry','replay','bell','seven','replay','bell','replay','bell','alt-seven','bell','replay','watermelon','bar','cherry','bell','replay','penguin','replay','bell','watermelon'],
    ['bell','replay','miss','seven','watermelon','replay','bar','miss','penguin','replay','alt-seven','miss','cherry','replay','bar','watermelon','miss','chance','bell','replay','miss'],
    ['replay','miss','bell','penguin','replay','bar','miss','replay','watermelon','chance','alt-seven','seven','bar','replay','miss','bell','replay','bar','miss','replay','miss']
  ];

  const forceOutcomeCodes = {
    miss:0,
    one_medal:1,
    bell9:2,
    bell15:3,
    replay:4,
    three_medal:15,
    weak_cherry:5,
    strong_cherry:6,
    watermelon:7,
    weak_chance:8,
    strong_chance:9,
    penguin_chance:10,
    hit:11,
    at:12,
    tier_up:13,
    freeze:14
  };

  const roleLabels = {
    miss:'ハズレ',
    one_medal:'1枚役',
    penguin_chance:'ペンギンチャンス',
    strong_chance:'強チャンス目',
    weak_chance:'弱チャンス目',
    weak_cherry:'弱チェリー',
    strong_cherry:'強チェリー',
    bell9:'ベル 9枚',
    bell15:'斜めベル 15枚',
    watermelon:'スイカ',
    replay:'リプレイ',
    three_medal:'3枚役',
    hit:'当たり',
    at:'AT',
    tier_up:'AT昇格',
    freeze:'フリーズ'
  };

  let Module;
  try {
    Module = await createSlotModule();
  } catch (err) {
    els.eventTitle.textContent = 'WASM LOAD ERROR';
    els.eventNote.textContent = 'slot.js / slot.wasm のビルドを確認してください';
    els.debug.textContent = String(err);
    return;
  }

  const callJson = (name, argTypes = [], args = []) => {
    const ptr = Module.ccall(name, 'number', argTypes, args);
    return JSON.parse(Module.UTF8ToString(ptr));
  };
  const state = () => callJson('slot_state_json');

  const labels = {
    normal_a:'通常A', normal_b:'通常B', heaven:'天国', super_heaven:'超天国', special:'特殊',
    lower:'下位', middle:'中位', upper:'上位', normal:'通常', specialized:'特化'
  };
  const eventLabels = {
    cz:'CZ', bonus:'BONUS', episode_bonus:'EPISODE BONUS',
    challenge_start:'AT CHALLENGE', challenge_point:'POINT', challenge_judge:'FINAL JUDGE',
    at_start:'AT START',
    at_add_games:'G数上乗せ', special_zone:'特化ZONE', upper_special_zone:'上位特化ZONE',
    stock_gain:'STOCK', tier_up:'昇格', tier_down:'転落', at_end:'AT END',
    upper_comeback:'上位引き戻し', freeze:'FREEZE', section_cross:'有利区間 CROSS',
    high_enter:'高確率 移行', high_exit:'高確率 終了', shorten:'G数短縮',
    cold_enter:'冷遇', section_reward:'有利区間ルーレット', at_omen:'AT 予兆'
  };

  let gameActive = false;
  let pendingResult = null;
  let pendingRole = 'miss';
  let pendingPhysicalRole = 'miss';
  let pendingPayout = 0;
  let pendingWasAT = false;
  let pendingWasBonus = false;
  let pendingWasChallenge = false;
  let pendingBellNaviActive = false;
  let pendingEntryReplayRole = false;
  let pendingStopOrder = [0, 1, 2];
  let pendingPressedOrder = [];
  let pendingNaviOrderValid = true;
  let pendingControl = null;
  let pendingGuaranteedLines = null;
  let deferredEntryReveal = null;
  let atOmenFlow = null;
  let pendingSyntheticEntry = false;
  let pendingEntryAlignFlag = false; // BONUS/AT開始待ち中、このGで図柄揃いを許可する1/2フラグ
  let pendingSyntheticOmen = false;
  let entryCinematicActive = false;
  let freezeSequenceActive = false;
  let pendingEntryAmbiguous = false;
  let reelTimers = [null, null, null];
  let reelStopped = [true, true, true];
  let reelPositions = [0, 0, 0];
  // 見た目は「終端のない循環ベルト」。
  // 通常は下方向へ連続移動し、フリーズ逆回転だけ上方向へ動かす。
  let reelArtFrames = [null, null, null];
  let reelArtPhase = [0, 0, 0];
  let reelArtDirection = [1, 1, 1]; // +1=下向き / -1=上向き
  let reelArtReady = [false, false, false];
  let reelGridCache = [null, null, null];
  let autoEnabled = false;
  let autoTimers = [];

  const diffHistory = [{ game:0, diff:0 }];
  let diffLastGame = 0;

  const recordDiffPoint = (s) => {
    const game = Number(s.totalGames || 0);
    const diff = Number(s.totalDiff || 0);
    const last = diffHistory[diffHistory.length - 1];
    if (!last || game !== last.game || diff !== last.diff) {
      diffHistory.push({ game, diff });
      diffLastGame = game;
      // 長時間プレイでも描画を重くしない。古い点だけ間引いて形状を維持。
      if (diffHistory.length > 1600) {
        const compact = [diffHistory[0]];
        for (let i = 1; i < diffHistory.length - 1; i += 2) compact.push(diffHistory[i]);
        compact.push(diffHistory[diffHistory.length - 1]);
        diffHistory.splice(0, diffHistory.length, ...compact);
      }
    }
  };

  const drawDiffGraph = () => {
    const canvas = els.diffGraph;
    if (!canvas) return;
    const rect = canvas.getBoundingClientRect();
    const dpr = Math.max(1, window.devicePixelRatio || 1);
    const w = Math.max(1, Math.round(rect.width * dpr));
    const h = Math.max(1, Math.round(rect.height * dpr));
    if (canvas.width !== w || canvas.height !== h) {
      canvas.width = w;
      canvas.height = h;
    }
    const ctx = canvas.getContext('2d');
    ctx.setTransform(dpr,0,0,dpr,0,0);
    const cw = rect.width, ch = rect.height;
    ctx.clearRect(0,0,cw,ch);

    const points = diffHistory.length ? diffHistory : [{game:0,diff:0}];
    const rawMin = Math.min(0, ...points.map(p => p.diff));
    const rawMax = Math.max(0, ...points.map(p => p.diff));
    const span = Math.max(100, rawMax - rawMin);
    const pad = Math.max(50, Math.ceil(span * 0.12));
    const minY = rawMin - pad;
    const maxY = rawMax + pad;
    const firstGame = points[0].game;
    const lastGame = Math.max(firstGame + 1, points[points.length - 1].game);

    const x = g => 7 + ((g - firstGame) / (lastGame - firstGame)) * Math.max(1, cw - 14);
    const y = v => 6 + ((maxY - v) / (maxY - minY)) * Math.max(1, ch - 12);

    ctx.lineWidth = 1;
    ctx.strokeStyle = 'rgba(164,178,210,.18)';
    for (let i = 1; i < 4; i++) {
      const yy = ch * i / 4;
      ctx.beginPath(); ctx.moveTo(0,yy); ctx.lineTo(cw,yy); ctx.stroke();
    }

    const zeroY = y(0);
    ctx.strokeStyle = 'rgba(255,255,255,.48)';
    ctx.lineWidth = 1;
    ctx.beginPath(); ctx.moveTo(0,zeroY); ctx.lineTo(cw,zeroY); ctx.stroke();

    if (points.length > 1) {
      const grad = ctx.createLinearGradient(0,0,cw,0);
      grad.addColorStop(0,'#61d5ff');
      grad.addColorStop(1,'#ffd45a');
      ctx.strokeStyle = grad;
      ctx.lineWidth = 2.2;
      ctx.lineJoin = 'round';
      ctx.lineCap = 'round';
      ctx.beginPath();
      points.forEach((p,i) => {
        const px=x(p.game), py=y(p.diff);
        if(i===0) ctx.moveTo(px,py); else ctx.lineTo(px,py);
      });
      ctx.stroke();
    }
  };

  const updateDiffPanel = (s) => {
    recordDiffPoint(s);
    const diff = Number(s.totalDiff || 0);
    const vals = diffHistory.map(p => p.diff);
    const max = Math.max(0, ...vals);
    const min = Math.min(0, ...vals);
    const signed = n => (n > 0 ? '+' : '') + n.toLocaleString();
    if (els.diffCurrent) els.diffCurrent.textContent = signed(diff);
    if (els.diffRange) els.diffRange.textContent = 'MAX ' + signed(max) + ' / MIN ' + signed(min);
    if (els.diffGames) els.diffGames.textContent = Number(s.totalGames || 0).toLocaleString() + 'G';
    drawDiffGraph();
  };

  window.addEventListener('resize', drawDiffGraph);

  const mod = (n, m) => ((n % m) + m) % m;
  const stopOrders = [
    [0,1,2],[0,2,1],[1,0,2],[1,2,0],[2,0,1],[2,1,0]
  ];

  const clearBellNavi = () => {
    pendingStopOrder = [0,1,2];
    pendingPressedOrder = [];
    pendingNaviOrderValid = true;
    stops.forEach((button) => {
      button.dataset.nav = '';
      button.classList.remove('nav-first','nav-active');
      const n = button.querySelector('.nav-order');
      if (n) n.textContent = '–';
    });
    if (els.naviState) els.naviState.textContent = 'NAVI ---';
  };

  const setBellNavi = (orderId) => {
    const safeId = Number.isInteger(orderId) && orderId >= 0 && orderId < stopOrders.length ? orderId : 0;
    pendingStopOrder = stopOrders[safeId];
    pendingStopOrder.forEach((reelIndex, orderIndex) => {
      const button = stops[reelIndex];
      button.dataset.nav = String(orderIndex + 1);
      button.classList.add('nav-active');
      if (orderIndex === 0) button.classList.add('nav-first');
      const n = button.querySelector('.nav-order');
      if (n) n.textContent = String(orderIndex + 1);
    });
    if (els.naviState) {
      const names = ['左','中','右'];
      els.naviState.textContent = '🔔 ' + pendingStopOrder.map(i => names[i]).join('→');
    }
  };

  const clearStageClasses = () => {
    els.effectLayer.className = 'effect-layer';
    els.cutinLayer.className = 'cutin-layer';
    els.stageScreen.classList.remove(
      'role-penguin','role-strong','role-hit','freeze-lock',
      'omen-white','omen-blue','omen-yellow','omen-green','omen-purple',
      'omen-red','omen-gold','omen-premium','screen-blackout','screen-shock'
    );
    if (els.omenText) {
      els.omenText.className = 'omen-text';
      els.omenText.textContent = '';
    }
  };

  const choosePresentation = (role, phase, result = pendingResult) => {
    const events = result?.events || [];
    const types = new Set(events.map(e => e.type));

    // プレミア系は確定時だけ。フェイク虹/フェイクストライプは禁止。
    if (role === 'entry_judge') return { cls:'omen-purple', fx:'purple', text:'どっちだ…' };
    if (role === 'freeze') return { cls:'omen-premium screen-blackout', fx:'stripe flash rainbow', text:'……', premium:true };
    if (role === 'at' || role === 'tier_up') return { cls:'omen-gold screen-shock', fx:'gold flash', text:'激熱' };
    if (role === 'hit') return { cls:'omen-red screen-shock', fx:'red flash slash', text:'好機' };

    if (types.has('at_omen')) {
      return { cls:'omen-purple', fx:'purple', text:'ざわ…' };
    }

    // 強役は赤以上、弱役は役色を中心に複数パターン。
    if (role === 'strong_cherry' || role === 'strong_chance') {
      return Math.random() < .25
        ? { cls:'omen-gold screen-shock', fx:'gold slash', text:'激熱' }
        : { cls:'omen-red screen-shock', fx:'red slash', text:'チャンス' };
    }
    if (role === 'penguin_chance') {
      // ペンギンチャンス自体は確定役ではない。プレミアストライプは禁止。
      return { cls:'omen-red screen-shock', fx:'red flash', text:'PENGUIN' };
    }
    if (role === 'weak_chance') {
      const r=Math.random();
      return r<.15 ? {cls:'omen-red',fx:'red',text:'チャンス'}
        : r<.55 ? {cls:'omen-purple',fx:'purple',text:'気配'}
        : {cls:'omen-blue',fx:'blue',text:''};
    }
    if (role === 'weak_cherry') {
      return Math.random()<.20
        ? {cls:'omen-yellow',fx:'yellow',text:'気配'}
        : {cls:'omen-green',fx:'green',text:''};
    }
    if (role === 'watermelon') {
      return Math.random()<.18
        ? {cls:'omen-yellow',fx:'yellow',text:'気配'}
        : {cls:'omen-green',fx:'green',text:''};
    }
    if (role === 'bell15') {
      return Math.random()<.12 ? {cls:'omen-yellow',fx:'yellow',text:''} : {cls:'',fx:'',text:''};
    }
    if (role === 'replay') {
      return Math.random()<.035 ? {cls:'omen-white',fx:'flash',text:'…'} : {cls:'',fx:'',text:''};
    }

    // 通常役はごく薄い違和感だけ。期待度を過剰に上げない。
    if (phase === 'spin' && Math.random()<.018) {
      return {cls:'omen-white',fx:'',text:'…'};
    }
    return {cls:'',fx:'',text:''};
  };

  const applyPresentation = (presentation) => {
    if (!presentation) return;
    if (presentation.cls) els.stageScreen.classList.add(...presentation.cls.split(' '));
    if (presentation.fx) els.effectLayer.className = 'effect-layer ' + presentation.fx;
    if (els.omenText && presentation.text) {
      els.omenText.textContent = presentation.text;
      els.omenText.classList.add('show');
    }
  };

  const stageCue = (role, phase = 'result') => {
    clearStageClasses();
    const presentation = choosePresentation(role, phase);
    applyPresentation(presentation);

    const cue = {
      entry_judge: { fx:'purple', title:'', sub:'', eyebrow:'' },
      one_medal: { fx:'', title:'1枚役', sub:'静かな払い出し', eyebrow:'NORMAL' },
      bell9: { fx:'yellow', title:'BELL', sub:'9枚', eyebrow:'YELLOW' },
      bell15: { fx:'gold', title:'BIG BELL', sub:'15枚', eyebrow:'GOLD' },
      replay: { fx:'blue', title:'REPLAY', sub:'もう一度', eyebrow:'BLUE' },
      three_medal: { fx:'', title:'3枚役', sub:'3枚', eyebrow:'NORMAL' },
      weak_cherry: { fx:'green', title:'CHERRY', sub:'弱チェリー', eyebrow:'GREEN' },
      strong_cherry: { fx:'red slash', title:'強チェリー', sub:'中段チェリー', eyebrow:'RED', cls:'role-strong' },
      weak_chance: { fx:'purple', title:'CHANCE', sub:'弱チャンス目', eyebrow:'PURPLE' },
      strong_chance: { fx:'red slash', title:'強チャンス目', sub:'🐧 🍒 🐧', eyebrow:'RED', cls:'role-strong' },
      penguin_chance: { fx:'red', title:'PENGUIN CHANCE', sub:'🐧 🐧 🐧', eyebrow:'RED', cls:'role-penguin' },
      hit: { fx:'red flash', title:'HIT', sub:'🟥7 🟥7 BAR', eyebrow:'RED', cls:'role-hit' },
      at: { fx:'gold flash', title:'AT START', sub:'🟥7 🟥7 🟥7', eyebrow:'GOLD', cls:'role-hit' },
      tier_up: { fx:'gold', title:'AT 昇格', sub:'BAR BAR BAR', eyebrow:'GOLD', cls:'role-hit' },
      freeze: { fx:'stripe flash', title:'FREEZE', sub:'🟦7 🟦7 🟦7', eyebrow:'PREMIUM', cls:'freeze-lock' },
      watermelon: { fx:'green', title:'WATERMELON', sub:'スイカ', eyebrow:'GREEN' },
      miss: { fx:'', title:'', sub:'', eyebrow:'' }
    }[role] || { fx:'', title:'', sub:'', eyebrow:'' };

    if (phase === 'result') {
      if (cue.cls) els.stageScreen.classList.add(...cue.cls.split(' '));
      if (cue.fx && !presentation.fx) els.effectLayer.className = 'effect-layer ' + cue.fx;
    }

    if (phase === 'spin') {
      if (role === 'freeze') {
        els.stageCaption.textContent = '……';
      } else if (['strong_cherry','strong_chance','penguin_chance','hit','at','tier_up'].includes(role)) {
        els.stageCaption.textContent = '気配がする…';
      }
      return;
    }

    if (role !== 'miss') {
      els.cutinEyebrow.textContent = cue.eyebrow;
      els.cutinTitle.textContent = cue.title;
      els.cutinSub.textContent = cue.sub;
      void els.cutinLayer.offsetWidth;
      els.cutinLayer.classList.add('show');
    }
  };

  const updateStageScene = (s) => {
    let scene = 'day';
    let caption = '昼ステージ';

    switch (s.normalMode) {
      case 'normal_a':
        scene = Number(s.normalPattern || 1) % 2 ? 'morning' : 'day';
        caption = scene === 'morning' ? '朝ステージ' : '昼ステージ';
        break;
      case 'normal_b':
        scene = 'evening';
        caption = '夕方ステージ';
        break;
      case 'heaven':
        scene = 'night';
        caption = '夜ステージ';
        break;
      case 'super_heaven':
        scene = 'deep-night';
        caption = '深夜ステージ';
        break;
      case 'special':
        scene = 'special';
        caption = '特殊ステージ';
        break;
      default:
        scene = 'day';
        caption = '通常ステージ';
    }

    if (s.inAT) {
      scene = s.atTier === 'upper' ? 'upper' : 'at';
      caption = s.atTier === 'upper' ? '上位AT' : 'AT';
    }
    if (s.inBonus) {
      scene = s.episodeBonus ? 'episode' : 'bonus';
      caption = s.episodeBonus ? 'EPISODE BONUS' : 'BONUS';
    } else if (s.challengeActive) {
      scene = 'challenge';
      caption = 'AT当選チャレンジ';
    }
    els.stageScreen.dataset.scene = scene;
    els.stageCaption.textContent = caption;
  };

  const visibleKind = (index, position, row) => {
    const strip = reelStrips[index];
    const offset = row - 1; // 0=上段, 1=中段, 2=下段
    return strip[mod(position + offset, strip.length)];
  };

  const prepareReelBelt = (index) => {
    const track = reelArtTracks[index];
    if (!track || reelArtReady[index]) return;

    const source = track.querySelector('img');
    if (!source || !source.complete || source.naturalWidth === 0) return;

    // DOMを21分割して並べる方式は廃止。
    // 元の縦リール画像をソースにして、canvasへ必要な3〜5コマだけ毎フレーム描画する。
    // これなら「継ぎ目」「ベルト端」「セル抜け」が物理的に存在しない。
    const canvas = document.createElement('canvas');
    canvas.className = 'reel-art-canvas';
    canvas.setAttribute('aria-hidden','true');
    track.appendChild(canvas);
    track.classList.add('canvas-ready');

    reelArtReady[index] = true;
  };

  const bootReelArtIfNeeded = (index) => {
    prepareReelBelt(index);
    if (!reelArtReady[index]) return;

    try {
      renderReelArt(index, true);
    } catch (err) {
      const track=reelArtTracks[index];
      track?.classList.remove('canvas-ready');
      const canvas=track?.querySelector('.reel-art-canvas');
      if (canvas) canvas.style.display='none';
      return;
    }

    if (reels[index].classList.contains('spinning') && reelArtFrames[index] === null) {
      startReelArtMotion(
        index,
        reels[index].classList.contains('reverse-spinning') ? -1 : 1
      );
    }
  };


  const reelArtGeometry = (index) => {
    const track = reelArtTracks[index];
    if (!track) return null;

    prepareReelBelt(index);
    if (!reelArtReady[index]) return null;

    const source = track.querySelector('img');
    const canvas = track.querySelector('.reel-art-canvas');
    if (!source || !canvas || !source.naturalWidth || !source.naturalHeight) return null;

    const width = reels[index].clientWidth;
    const height = reels[index].clientHeight;
    if (!width || !height) return null;

    const dpr = Math.min(2,Math.max(1,window.devicePixelRatio||1));
    const pixelW=Math.max(1,Math.round(width*dpr));
    const pixelH=Math.max(1,Math.round(height*dpr));
    if (canvas.width!==pixelW || canvas.height!==pixelH) {
      canvas.width=pixelW;
      canvas.height=pixelH;
    }
    canvas.style.width=width+'px';
    canvas.style.height=height+'px';

    const cellHeight=height/3;
    const stripHeight=cellHeight*21;

    // 実画像で測った「21コマ本体」の上下端。
    // 左/中は512x1536系、右は724x2172系だが比率で持つので実PNGサイズに依存しない。
    const verticalBand=[
      { top:8/1536,  bottom:1483/1536 },
      { top:8/1536,  bottom:1482/1536 },
      { top:11/2172, bottom:2058/2172 }
    ][index];

    const sy0=source.naturalHeight*verticalBand.top;
    const sy1=source.naturalHeight*verticalBand.bottom;
    const srcCellHeight=(sy1-sy0)/21;

    // 白余白は捨て、レール本体だけを使う。
    const sx=source.naturalWidth*(index===2 ? 0.275 : 0.300);
    const sw=source.naturalWidth*(index===2 ? 0.450 : 0.400);

    return {
      track,source,canvas,ctx:canvas.getContext('2d'),
      width,height,dpr,cellHeight,stripHeight,
      sx,sw,sy0,srcCellHeight
    };
  };

  const targetReelArtPhase = (index) => {
    const geo = reelArtGeometry(index);
    if (!geo) return null;

    // 停止時は必ず、上段=position-1 / 中段=position / 下段=position+1。
    const topLogical = mod(reelPositions[index] - 1, 21);
    return -(topLogical * geo.cellHeight);
  };


  const pressedPositionAtPayline = (index) => {
    const geo = reelArtGeometry(index);
    if (!geo || !Number.isFinite(reelArtPhase[index])) {
      return mod(reelPositions[index],reelStrips[index].length);
    }

    // 赤ライン中央に最も近い図柄を、その瞬間に押した位置として扱う。
    // BARを赤ライン中央で押したなら、内部基準も必ずそのBARになる。
    const raw = 1 - (reelArtPhase[index] / geo.cellHeight);
    return mod(Math.round(raw),reelStrips[index].length);
  };

  const paintReelArt = (index) => {
    const geo = reelArtGeometry(index);
    if (!geo || !geo.ctx) return;

    const {
      ctx,source,width,height,dpr,cellHeight,
      sx,sw,sy0,srcCellHeight
    }=geo;

    ctx.setTransform(dpr,0,0,dpr,0,0);
    ctx.clearRect(0,0,width,height);
    ctx.fillStyle='#f7f9fc';
    ctx.fillRect(0,0,width,height);
    ctx.imageSmoothingEnabled=true;
    ctx.imageSmoothingQuality='high';

    const phase=reelArtPhase[index];
    const firstK=Math.floor((-phase)/cellHeight)-2;
    const lastK=firstK+8;

    try {
      for(let k=firstK;k<=lastK;k++){
        const logical=mod(k,21);
        const sy=sy0+logical*srcCellHeight;
        const dy=phase+k*cellHeight;

        // 各コマの中心を destination cell の中心へ固定。
        // 0.7pxだけ重ねてRetinaの境界線も消す。
        ctx.drawImage(
          source,
          sx,sy,sw,srcCellHeight,
          -0.5,dy-0.7,width+1,cellHeight+1.4
        );
      }
    } catch(err) {
      geo.track.classList.remove('canvas-ready');
      geo.canvas.style.display='none';
    }
  };

  const renderReelArt = (index, immediate = false) => {
    if (reelArtFrames[index] !== null && !immediate) return;

    const phase = targetReelArtPhase(index);
    if (phase === null) return;

    reelArtPhase[index] = phase;
    normalizeReelArtPhase(index);
    paintReelArt(index);
  };

  const cancelReelArtFrame = (index) => {
    if (reelArtFrames[index] !== null) {
      cancelAnimationFrame(reelArtFrames[index]);
      reelArtFrames[index] = null;
    }
  };

  const normalizeReelArtPhase = (index) => {
    const geo = reelArtGeometry(index);
    if (!geo) return;

    // canvas上では21コマごとに完全同一なので、ここで±1周しても画面は1pxも変わらない。
    const cycle = geo.stripHeight;
    while (reelArtPhase[index] > 0) reelArtPhase[index] -= cycle;
    while (reelArtPhase[index] <= -cycle) reelArtPhase[index] += cycle;
  };

  const startReelArtMotion = (index, direction) => {
    cancelReelArtFrame(index);

    const geo = reelArtGeometry(index);
    if (!geo) return;

    reelArtDirection[index] = direction;

    if (!Number.isFinite(reelArtPhase[index])) {
      reelArtPhase[index] = targetReelArtPhase(index) ?? 0;
    }
    normalizeReelArtPhase(index);

    // 少しだけ速度を落とす。通常は約42ms/1コマ、逆回転は約36ms/1コマ。
    // 高速感は残しつつ、絵柄を追える程度に調整する。
    const secondsPerCell = direction > 0 ? 0.042 : 0.036;
    const speed = geo.cellHeight / secondsPerCell;
    let last = performance.now();

    const frame = (now) => {
      const currentGeo = reelArtGeometry(index);
      if (!currentGeo) {
        reelArtFrames[index] = requestAnimationFrame(frame);
        return;
      }

      const dt = Math.min(0.035, Math.max(0,(now-last)/1000));
      last = now;

      reelArtPhase[index] += direction * speed * dt;
      normalizeReelArtPhase(index);
      paintReelArt(index);

      reelArtFrames[index] = requestAnimationFrame(frame);
    };

    reelArtFrames[index] = requestAnimationFrame(frame);
  };

  const settleReelArt = (index, pressedBase, finalPosition) => {
    cancelReelArtFrame(index);

    const geo=reelArtGeometry(index);
    if(!geo) return;

    const exact=targetReelArtPhase(index);
    if(exact===null) return;

    const current=reelArtPhase[index];
    const cycle=geo.stripHeight;
    const dir=reelArtDirection[index]>=0 ? 1 : -1;

    // exact と同じ停止位置を、現在の回転方向の「次に来る位置」へ展開する。
    // finalPosition は pressedBase から最大4コマなので、ここで21コマ一周することはない。
    let target=exact;
    if(dir>0){
      while(target<current) target+=cycle;
    }else{
      while(target>current) target-=cycle;
    }

    const cells=Math.abs(target-current)/geo.cellHeight;

    const exactAssist = !!pendingControl?.exactAssistUsed?.[index];

    // 通常役は0〜4コマの範囲内。
    // ベル/REPLAYの専用アシストだけは、他図柄へ逃がさず次の同図柄まで止める。
    if(cells>5.05 && !exactAssist){
      reelArtPhase[index]=exact;
      normalizeReelArtPhase(index);
      paintReelArt(index);
      return;
    }

    const from=current;
    const duration = exactAssist
      ? Math.max(90, Math.min(320, cells * 24))
      : Math.max(38, Math.min(175, cells * 42 + 18));
    const started=performance.now();

    // ほぼ一定速で滑り、最後の約18%だけ軽く減速。
    const stopCurve=(t)=>{
      if(t<=0.82) return (t/0.82)*0.90;
      const u=(t-0.82)/0.18;
      return 0.90+0.10*(1-Math.pow(1-u,2));
    };

    const frame=(now)=>{
      const t=Math.min(1,(now-started)/duration);
      const p=stopCurve(t);
      reelArtPhase[index]=from+(target-from)*p;
      paintReelArt(index);

      if(t<1){
        reelArtFrames[index]=requestAnimationFrame(frame);
      }else{
        reelArtFrames[index]=null;
        // target 自体が exact と同位相なので、ここでの補正移動は発生しない。
        reelArtPhase[index]=target;
        normalizeReelArtPhase(index);
        paintReelArt(index);
      }
    };

    reelArtFrames[index]=requestAnimationFrame(frame);
  };

  const renderReel = (index) => {
    const position = reelPositions[index];
    const spans = [...reels[index].querySelectorAll(':scope > span')];
    spans.forEach((el, row) => {
      const symbol = symbolByKind[visibleKind(index, position, row)];
      el.dataset.kind = symbol.kind;
      el.innerHTML = symbol.html;
    });
    renderReelArt(index);
  };


  window.addEventListener('resize', () => {
    for (let i = 0; i < 3; i++) {
      if (reelArtFrames[i] === null) renderReelArt(i, true);
    }
  });

  reelArtTracks.forEach((track, index) => {
    const img = track?.querySelector('img');
    if (!img) return;

    if (img.complete && img.naturalWidth > 0) {
      bootReelArtIfNeeded(index);
    } else {
      img.addEventListener('load', () => bootReelArtIfNeeded(index), { once:true });
      img.addEventListener('error', () => {
        // 読み込み失敗時は下の既存文字リールを見せ、画面自体を消さない。
        track.closest('.reel-art')?.classList.add('reel-art-failed');
      }, { once:true });
    }
  });

  const centerKind = (index, position = reelPositions[index]) => visibleKind(index, position, 1);

  const findExactPosition = (index, kind, row = 1) => {
    const strip = reelStrips[index];
    for (let pos = 0; pos < strip.length; pos++) {
      if (visibleKind(index, pos, row) === kind) return pos;
    }
    return reelPositions[index];
  };

  const payoutLinesForPositions = (positions = reelPositions) => {
    const rowLine = (row) => [0,1,2].map(i => visibleKind(i, positions[i], row));
    return {
      top: rowLine(0),
      center: rowLine(1),
      bottom: rowLine(2),
      diagUp: [
        visibleKind(0, positions[0], 2),
        visibleKind(1, positions[1], 1),
        visibleKind(2, positions[2], 0)
      ],
      diagDown: [
        visibleKind(0, positions[0], 0),
        visibleKind(1, positions[1], 1),
        visibleKind(2, positions[2], 2)
      ]
    };
  };

  const payoutLineRows = {
    top:[0,0,0],
    center:[1,1,1],
    bottom:[2,2,2],
    diagUp:[2,1,0],
    diagDown:[0,1,2]
  };

  const classifyPhysicalLinePayout = (positions = reelPositions) => {
    const lines = payoutLinesForPositions(positions);
    const all = [lines.top, lines.center, lines.bottom, lines.diagUp, lines.diagDown];

    // ベルは停止ラインごとに払い出しを固定。
    // 右上がり（左下→中中→右上）=15枚
    // 中段=9枚
    // 右下がり（左上→中中→右下）/上段/下段=3枚
    if (lines.diagUp.every(k => k === 'bell')) return 'bell15';
    if (lines.center.every(k => k === 'bell')) return 'bell9';
    if (
      lines.diagDown.every(k => k === 'bell')
      || lines.top.every(k => k === 'bell')
      || lines.bottom.every(k => k === 'bell')
    ) return 'three_medal';

    // リプレイは5ラインのどこで揃っても有効。
    if (all.some(line => line.every(k => k === 'replay'))) return 'replay';
    return null;
  };

  const classifyPattern = (positions = reelPositions) => {
    const lines = payoutLinesForPositions(positions);
    const center = lines.center;

    // 7/ボーナス図柄、レア役、チャンス目は中段メインライン基準。
    if (center.every(k => k === 'alt-seven')) return 'freeze';
    if (center.every(k => k === 'seven')) return 'at';
    if (center.every(k => k === 'bar')) return 'tier_up';
    if (center[0] === 'seven' && center[1] === 'seven' && center[2] === 'bar') return 'hit';

    if (center.every(k => k === 'penguin')) return 'penguin_chance';
    if (center[0] === 'penguin' && center[1] === 'cherry' && center[2] === 'penguin') return 'strong_chance';
    if (center.filter(k => k === 'penguin').length === 2) return 'weak_chance';

    const leftVisible = [0,1,2].map(row => visibleKind(0, positions[0], row));
    const middleVisible = [0,1,2].map(row => visibleKind(1, positions[1], row));
    const rightVisible = [0,1,2].map(row => visibleKind(2, positions[2], row));

    const leftHasCherry = leftVisible.includes('cherry');
    const hasVisibleReplay =
      leftVisible.includes('replay')
      || middleVisible.includes('replay')
      || rightVisible.includes('replay');

    // 弱チェ：左リールに🍒が見え、停止窓内にREPLAYが1つでもある。
    // 強チェ：左中段🍒かつ、停止窓内にREPLAYが1つもない。
    if (leftHasCherry && hasVisibleReplay) return 'weak_cherry';
    if (center[0] === 'cherry' && !hasVisibleReplay) return 'strong_cherry';

    // スイカなどのレア役はメインライン基準。取りこぼし時だけ代用停止を使う。
    if (center.every(k => k === 'watermelon')) return 'watermelon';

    // ベル／リプレイの払出ライン判定はメイン役とは独立。
    return classifyPhysicalLinePayout(positions) || 'miss';
  };

  const accountingReturnForRole = (role) => {
    if (role === 'one_medal') return 1;
    if (role === 'bell9') return 9;
    if (role === 'bell15') return 15;
    if (role === 'replay') return 3; // 3枚BETを差枚会計上相殺
    if (role === 'three_medal') return 3;
    if (role === 'hit' || role === 'at' || role === 'tier_up' || role === 'freeze') return 3; // 7/BONUS図柄はリプレイ
    return 0;
  };

  const bonusRareRoles = new Set([
    'weak_cherry','watermelon','weak_chance','strong_chance'
  ]);

  // BONUS / EPISODE の非レアG専用分布。
  // 3枚BETに対する平均払出は 9枚 = 平均純増+6枚。
  // 1枚役・ハズレはBONUS中には出さない。
  const chooseBonusSafeRole = () => {
    const r = Math.random();
    if (r < 0.50) return 'bell9';
    if (r < 0.75) return 'bell15';
    if (r < 0.875) return 'replay';
    return 'three_medal';
  };

  const bonusVisibleReturnForRole = (role) => {
    if (bonusRareRoles.has(role)) return 3; // レア役Gも減らさない
    return accountingReturnForRole(role);
  };

  const chooseEntryWaitPhysicalRole = () => {
    // BONUS/AT開始待ち中も、見た目は通常遊技として成立役を出す。
    // 内部BONUS/AT状態は進めず、停止制御用の物理役だけを独立抽選する。
    const r = Math.random();
    let x = 0;

    const table = [
      ['one_medal', 4/5],
      ['bell9', 1/15],
      ['replay', 1/30],
      ['bell15', 1/80],
      ['weak_chance', 1/90],
      ['watermelon', 1/100],
      ['weak_cherry', 1/180],
      ['strong_chance', 1/150],
      ['penguin_chance', 1/500],
      ['strong_cherry', 1/1000]
    ];

    for (const [role, p] of table) {
      x += p;
      if (r < x) return role;
    }
    return 'miss';
  };

  const deriveRoleFromResult = (result, physicalFirst = false) => {
    const forced = els.roleTest.value;
    if (forced) return forced;

    const reelRole = result.reelRole || 'miss';
    if (physicalFirst) return reelRole;
    const rarePhysical = new Set([
      'weak_cherry','strong_cherry','watermelon',
      'weak_chance','strong_chance','penguin_chance'
    ]);
    // レア役成立ゲームは、報酬イベントより実際の停止形を優先表示する。
    if (rarePhysical.has(reelRole)) return reelRole;

    const events = result.events || [];
    const types = new Set(events.map(e => e.type));
    if (types.has('freeze')) return 'freeze';
    if (types.has('bonus') || types.has('episode_bonus')) return 'hit';
    if (types.has('tier_up')) return 'tier_up';
    if (types.has('at_start')) return 'at';

    return reelRole;
  };

  const targetForRole = (role, index, threeMedalLine = null) => {
    switch (role) {
      // ここは「その図柄を明示的に狙わせる演出」用の停止目。
      // 通常の内部AT/BONUS当選はこの表を使わず、成立した物理役の制御を使う。
      case 'freeze': return { row:1, kind:'alt-seven' };
      case 'at': return { row:1, kind:'seven' };
      case 'tier_up': return { row:1, kind:'bar' };
      case 'hit': return { row:1, kind:index === 2 ? 'bar' : 'seven' };
      case 'penguin_chance': return { row:1, kind:'penguin' };
      case 'strong_chance': return { row:1, kind:index === 1 ? 'cherry' : 'penguin' };
      case 'weak_chance': return index < 2 ? { row:1, kind:'penguin' } : null;
      case 'weak_cherry':
        if (index === 0) return { row:2, kind:'cherry' };
        if (index === 1) return { row:1, kind:'replay' };
        return null;
      case 'strong_cherry':
        if (index === 0) return { row:1, kind:'cherry' };
        return null;
      case 'bell9': return { row:1, kind:'bell' };
      case 'bell15':
        return { row:index === 0 ? 2 : index === 1 ? 1 : 0, kind:'bell' };
      case 'watermelon': return { row:1, kind:'watermelon' };
      case 'replay': return { row:1, kind:'replay' };
      case 'three_medal':
        if (threeMedalLine === 'top') return { row:0, kind:'bell' };
        if (threeMedalLine === 'bottom') return { row:2, kind:'bell' };
        // 15枚役と反対の斜め：左上→中中→右下
        return { row:index === 0 ? 0 : index === 1 ? 1 : 2, kind:'bell' };
      default: return null;
    }
  };

  const candidateMatchesTarget = (index, position, target) => {
    if (!target) return true;
    const kind = visibleKind(index, position, target.row);
    if (target.kind) return kind === target.kind;
    if (target.notKind) return kind !== target.notKind;
    return true;
  };

  // 基本は目押し。成立図柄が押下位置から0〜4コマにあれば通常の引き込みで取れる。
  // そのうえで現代機側の救済として、指定役だけ「本図柄を外した時の代用停止」を許可する。
  // BARはチェリー/スイカを狙うためのランドマークなので、代用停止先には使わない。
  const assistSubstituteRoles = new Set([
    'watermelon','weak_chance','strong_chance','penguin_chance'
  ]);

  // AT/BONUSなどの内部当選をリール側へ強制表示するアシストは使わない。
  const aimAssistRoles = new Set();

  const buildSpinControl = (role, result) => {
    // 3枚役は「反対斜め / 上段 / 下段」の3停止形のいずれか。
    // 成立GのレバーON時に停止形を1つ固定し、STOP中には変更しない。
    const threeMedalLines = ['diagDown','top','bottom'];
    const threeMedalLine = role === 'three_medal'
      ? threeMedalLines[Math.floor(Math.random() * threeMedalLines.length)]
      : null;

    return {
      role,
      // 制御テーブルはレバーON時点で固定。STOP時は押下位置からこの表を参照するだけ。
      targets: [0,1,2].map(index => targetForRole(role, index, threeMedalLine)),
      threeMedalLine,
      // hybrid: manual pull-in first, substitute rescue second.
      manualFirst: true,
      assistSubstitute: assistSubstituteRoles.has(role),
      substituteUsed: [false, false, false],
      exactAssistUsed: [false, false, false],
      aimAssist: aimAssistRoles.has(role),
      aimAssistUsed: [false, false, false],
      navOrder: Number(result.navOrder ?? -1)
    };
  };

  const slipPosition = (index, base, slip) =>
    mod(base - slip, reelStrips[index].length);

  const chooseAssistSubstitutePosition = (index, base) => {
    // 代用停止も押下位置から0〜4コマ。最小スベリを最優先する。
    // BAR/CHANCE/×のどれかへ無理に大きく滑らせない。
    const safeKinds = new Set(['chance','miss']);
    for (let slip = 0; slip <= 4; slip++) {
      const candidate = slipPosition(index,base,slip);
      if (safeKinds.has(visibleKind(index,candidate,1))) {
        pendingControl.substituteUsed[index] = true;
        return candidate;
      }
    }
    pendingControl.substituteUsed[index] = true;
    return base;
  };

  const guaranteedPayoutRoles = new Set(['bell9','bell15','three_medal','replay']);

  const guaranteedSymbolForRole = (role) => {
    if (role === 'replay') return 'replay';
    if (role === 'bell9' || role === 'bell15' || role === 'three_medal') return 'bell';
    return null;
  };

  const guaranteedLinesForRole = (role) => {
    if (role === 'bell9') return ['center'];
    if (role === 'bell15') return ['diagUp'];
    if (role === 'three_medal') {
      const line = pendingControl?.threeMedalLine;
      return line ? [line] : ['diagDown','top','bottom'];
    }
    if (role === 'replay') return ['center'];
    return [];
  };

  const chooseGuaranteedPayoutPosition = (index, target, base) => {
    const role = pendingControl?.role;
    const symbol = guaranteedSymbolForRole(role);
    if (!symbol) return base;

    if (!Array.isArray(pendingGuaranteedLines) || !pendingGuaranteedLines.length) {
      pendingGuaranteedLines = guaranteedLinesForRole(role);
    }

    const candidatesForSlip = (slip) => {
      const candidate = slipPosition(index, base, slip);
      const lines = pendingGuaranteedLines.filter(name => {
        const rows = payoutLineRows[name];
        if (!rows) return false;

        // ベル/REPLAYは、その図柄そのものでしか成立させない。
        if (visibleKind(index, candidate, rows[index]) !== symbol) return false;

        for (let j = 0; j < 3; j++) {
          if (j === index || !reelStopped[j]) continue;
          if (visibleKind(j, reelPositions[j], rows[j]) !== symbol) return false;
        }
        return true;
      });
      return { candidate, lines };
    };

    // まず通常の0〜4コマ引き込み。
    for (let slip = 0; slip <= 4; slip++) {
      const hit = candidatesForSlip(slip);
      if (hit.lines.length) {
        pendingGuaranteedLines = hit.lines;
        return hit.candidate;
      }
    }

    // ベル/REPLAYは代用図柄やハズレへ逃がさない。
    // 現代機側のアシスト領域として、同じ成立図柄の次の停止位置まで制御する。
    for (let slip = 5; slip < reelStrips[index].length; slip++) {
      const hit = candidatesForSlip(slip);
      if (hit.lines.length) {
        pendingGuaranteedLines = hit.lines;
        pendingControl.exactAssistUsed[index] = true;
        return hit.candidate;
      }
    }

    // 配列上にも成立図柄が作れない異常時だけ現位置。
    return base;
  };

  const chooseAimAssistedPosition = (index, target, base) => {
    for (let slip = 0; slip <= 4; slip++) {
      const candidate = slipPosition(index, base, slip);
      if (candidateMatchesTarget(index, candidate, target)) {
        pendingControl.aimAssistUsed[index] = slip > 0;
        return candidate;
      }
    }
    return base;
  };

  const chooseNavigatedBellPosition = (index, base) => {
    // 押し順正解時は必ず🔔そのもので中段成立。
    // 他図柄をベル扱いすることは絶対にしない。
    pendingGuaranteedLines = ['center'];
    return chooseGuaranteedPayoutPosition(index, { row:1, kind:'bell' }, base);
  };

  const middleCherryMiddleReelSafe = (position) => {
    // 中段チェリー成立Gは、中リールの可視3コマすべてからREPLAYを排除する。
    // 5本の有効払出ラインは必ず中リールの上/中/下のいずれかを通るため、
    // これでリプレイ揃いそのものを物理的に不可能にする。
    return [0,1,2].every(row => visibleKind(1, position, row) !== 'replay');
  };

  const chooseMiddleCherryPosition = (index, base) => {
    if (index === 0) {
      for (let slip = 0; slip <= 4; slip++) {
        const candidate = slipPosition(index, base, slip);
        if (visibleKind(0, candidate, 1) === 'cherry') return candidate;
      }
      return base;
    }

    if (index === 1) {
      for (let slip = 0; slip <= 4; slip++) {
        const candidate = slipPosition(index, base, slip);
        if (middleCherryMiddleReelSafe(candidate)) return candidate;
      }
      // 4コマ以内にREPLAY無し停止形を作れない場合は無理に強チェ扱いしない。
      // 実際に止まった窓のREPLAY有無で弱/強を判定する。
      return base;
    }

    return base;
  };

  const missCenterSafe = (index, position) => {
    const kind = visibleKind(index, position, 1);

    // 通常時ハズレで中段にこれらを見せると、成立していないレア役/チャンス目の煽りになる。
    // 🍉を左中段に止めないことでスイカ狙いの煽りを開始させず、
    // 🐧を中段に止めないことで弱/強チャンス目・ペンギン揃いの煽りも作らない。
    if (kind === 'watermelon' || kind === 'penguin') return false;

    // 中リール中段🍒も、左右🐧との強チャンス目を想起させるためハズレでは避ける。
    if (index === 1 && kind === 'cherry') return false;

    return true;
  };

  const chooseNormalMissPosition = (index, base) => {
    const willAllStop = reelStopped.filter(Boolean).length === 2;

    // 通常ハズレの1・2停止目は押した位置をそのまま使う。
    // BARやその周辺の配置を「目印」として作ったリール配列を崩さない。
    if (!willAllStop) return base;

    // 最終停止だけ、ハズレなのに偶然役が完成する場合に限り0〜4コマで蹴る。
    for (let slip = 0; slip <= 4; slip++) {
      const candidate = slipPosition(index,base,slip);
      const test=[...reelPositions];
      test[index]=candidate;
      if (classifyPattern(test)==='miss') return candidate;
    }

    return base;
  };

  const rightJudgePair = () => {
    const strip = reelStrips[2];
    for (let i = 0; i < strip.length; i++) {
      const next = mod(i + 1, strip.length);
      if (strip[i] === 'seven' && strip[next] === 'bar') {
        return { seven:i, bar:next };
      }
      if (strip[i] === 'bar' && strip[next] === 'seven') {
        return { seven:next, bar:i };
      }
    }
    return null;
  };

  const chooseAmbiguousRightJudgePosition = (role) => {
    const pair = rightJudgePair();
    if (!pair) return findExactPosition(2, role === 'at' ? 'seven' : 'bar', 1);
    return role === 'at' ? pair.seven : pair.bar;
  };

  const chooseStopPosition = (index, navigatedBell = false, naviMiss = false, pressedBase = null) => {
    const strip = reelStrips[index];
    const base = pressedBase === null
      ? mod(reelPositions[index], strip.length)
      : mod(pressedBase, strip.length);

    if (navigatedBell) {
      return chooseNavigatedBellPosition(index, base);
    }

    // AT押し順を外した後は、内部成立ベルの引き込み制御を使わない。
    // 実際に押したタイミングの停止形をそのまま「外した役」として扱う。
    if (naviMiss) {
      return base;
    }

    if (!pendingControl) return base;

    // strong_cherry は現行ゲーム仕様上「中段チェリー」。
    // レバーONで成立確定しているため、汎用取りこぼし/代用制御を通さない。
    if (pendingControl.role === 'strong_cherry') {
      return chooseMiddleCherryPosition(index, base);
    }

    // 通常時ハズレ/1枚役の見た目は、最初のSTOPからレア役煽りを作らない。
    if (!pendingWasAT && !pendingWasBonus && !pendingWasChallenge
        && (pendingControl.role === 'miss' || pendingControl.role === 'one_medal')) {
      return chooseNormalMissPosition(index, base);
    }

    const target = pendingControl.targets[index];
    if (target) {
      if (guaranteedPayoutRoles.has(pendingControl.role)) {
        return chooseGuaranteedPayoutPosition(index, target, base);
      }

      // まず実機の基本となる0〜4コマ引き込み。
      // ここは「アシスト」ではなく、押した場所に応じた通常の停止制御。
      for (let slip = 0; slip <= 4; slip++) {
        const candidate = slipPosition(index, base, slip);
        if (candidateMatchesTarget(index, candidate, target)) return candidate;
      }

      if (pendingControl.aimAssist) {
        return chooseAimAssistedPosition(index, target, base);
      }

      // 指定された救済役だけ、本図柄が4コマ圏外なら代用停止へ落とす。
      // それ以外は目押し結果をそのまま反映して取りこぼす。
      if (pendingControl.assistSubstitute) {
        return chooseAssistSubstitutePosition(index, base);
      }
      return base;
    }

    // 1枚役は専用停止形をまだ持たないので、見た目はハズレ系停止形へ逃がす。
    const physicalRole = pendingControl.role === 'one_medal' ? 'miss' : pendingControl.role;

    // ハズレ/任意リールは、最後の停止で予約済み停止形を偶然完成させない候補を優先。
    const willAllStop = reelStopped.filter(Boolean).length === 2;
    if (willAllStop) {
      for (let slip = 0; slip <= 4; slip++) {
        const candidate = slipPosition(index, base, slip);
        const test = [...reelPositions];
        test[index] = candidate;
        if (physicalRole === 'miss' && classifyPattern(test) === 'miss') return candidate;
        if (physicalRole !== 'miss' && classifyPattern(test) === physicalRole) return candidate;
      }
    }

    return base;
  };

  const clearAutoTimers = () => {
    autoTimers.forEach(clearTimeout);
    autoTimers = [];
  };

  const startReelMotion = (index) => {
    prepareReelBelt(index);

    reelStopped[index] = false;
    reels[index].classList.remove('reverse-spinning');
    reels[index].classList.add('spinning');
    stops[index].disabled = false;
    stops[index].classList.add('active');

    // 通常時は必ず見た目が下向きに流れる。
    // まだ画像ロード前なら、loadイベント側のbootReelArtIfNeededが起動を引き継ぐ。
    startReelArtMotion(index, 1);

    reelTimers[index] = setInterval(() => {
      reelPositions[index] = mod(reelPositions[index] - 1, reelStrips[index].length);
      renderReel(index);
    }, 42);
  };

  const startReverseReelMotion = (index) => {
    prepareReelBelt(index);

    reelStopped[index] = false;
    reels[index].classList.add('spinning','reverse-spinning');
    stops[index].disabled = true;
    stops[index].classList.remove('active');

    // フリーズなどの逆回転演出だけ上向き。
    startReelArtMotion(index, -1);

    reelTimers[index] = setInterval(() => {
      reelPositions[index] = mod(reelPositions[index] + 1, reelStrips[index].length);
      renderReel(index);
    }, 36);
  };

  const stopAllReelTimers = () => {
    for (let i = 0; i < 3; i++) {
      if (reelTimers[i]) {
        clearInterval(reelTimers[i]);
        reelTimers[i] = null;
      }
      cancelReelArtFrame(i);
      reels[i].classList.remove('spinning','reverse-spinning');
    }
  };

  const showMachineCinematic = (eyebrow, title, sub, cls = '') => {
    els.machineCinematic.className = 'machine-cinematic show ' + cls;
    els.cinematicEyebrow.textContent = eyebrow || '';
    els.cinematicTitle.textContent = title || '';
    els.cinematicSub.textContent = sub || '';
  };

  const hideMachineCinematic = () => {
    els.machineCinematic.className = 'machine-cinematic';
    els.cinematicEyebrow.textContent = '';
    els.cinematicTitle.textContent = '';
    els.cinematicSub.textContent = '';
  };

  const unlockEntryStops = () => {
    for (let i = 0; i < 3; i++) {
      if (!reelTimers[i]) startReelMotion(i);
      stops[i].disabled = false;
      stops[i].classList.add('active');
    }
    if (autoEnabled) {
      clearAutoTimers();
      [0,1,2].forEach((reelIndex, i) => {
        autoTimers.push(setTimeout(() => stopReelMotion(reelIndex), 650 + i * 300));
      });
    }
  };

  const runNormalEntryJudgeCinematic = () => {
    entryCinematicActive = true;

    // 通常当選の答えは演出文字では出さない。
    // 左・中は必ず🟥7、右リールだけが内部結果に応じて🟥7/BARへ着地する。
    for (let i = 0; i < 3; i++) {
      startReelMotion(i);
      stops[i].disabled = true;
      stops[i].classList.remove('active');
    }

    els.eventTitle.textContent = '当たり';
    els.eventNote.textContent = 'まだ行き先は分からない';
    showMachineCinematic('HIT', '当たり', '右リールで決まる', 'judge');

    autoTimers.push(setTimeout(() => {
      showMachineCinematic('TARGET', 'STOPでジャッジ', '内部結果は最終停止で告知', 'judge target');
      els.stageScreen.classList.add('omen-purple');
      els.effectLayer.className = 'effect-layer purple';
      els.eventTitle.textContent = 'STOPでジャッジ';
      els.eventNote.textContent = 'リールは成立役どおりに停止 / 行き先は内部結果で告知';
      entryCinematicActive = false;
      unlockEntryStops();
    }, 850));
  };

  const runBonusEntryCinematic = (role) => {
    entryCinematicActive = true;
    for (let i = 0; i < 3; i++) {
      startReelMotion(i);
      stops[i].disabled = true;
      stops[i].classList.remove('active');
    }

    const isAT = role === 'at';
    els.eventTitle.textContent = isAT ? 'AT 確定' : 'BONUS 確定';
    els.eventNote.textContent = '図柄告知待機';
    showMachineCinematic(
      isAT ? 'AT CONFIRMED' : 'BONUS CONFIRMED',
      isAT ? 'AT 確定' : 'BONUS 確定',
      '',
      isAT ? 'gold' : 'red'
    );

    autoTimers.push(setTimeout(() => {
      showMachineCinematic(
        'TARGET',
        'STOP',
        isAT ? 'AT内部当選' : 'BONUS内部当選',
        isAT ? 'gold target' : 'red target'
      );
      els.eventTitle.textContent = 'STOP';
      els.eventNote.textContent = '内部当選済み / リールは成立役どおりに停止';
      entryCinematicActive = false;
      unlockEntryStops();
    }, 900));
  };

  const runFreezeEntryCinematic = () => {
    entryCinematicActive = true;
    freezeSequenceActive = true;
    els.machine.classList.add('freeze-all-off');
    els.eventTitle.textContent = '……';
    els.eventNote.textContent = '';
    clearStageClasses();
    hideMachineCinematic();

    // 全灯消灯と同時に3リールが逆回転。STOPはまだ受け付けない。
    for (let i = 0; i < 3; i++) startReverseReelMotion(i);

    autoTimers.push(setTimeout(() => {
      els.machine.classList.remove('freeze-all-off');
      els.machine.classList.add('freeze-return');
      showMachineCinematic('SYSTEM', 'PREMIUM FREEZE', '異常回転', 'freeze');
      els.stageScreen.classList.add('freeze-lock','omen-premium');
      els.effectLayer.className = 'effect-layer stripe flash';
    }, 700));

    autoTimers.push(setTimeout(() => {
      stopAllReelTimers();
      for (let i = 0; i < 3; i++) startReelMotion(i);
      for (let i = 0; i < 3; i++) {
        stops[i].disabled = true;
        stops[i].classList.remove('active');
      }
      showMachineCinematic('TARGET', '🟦7を狙え', '🟦7 🟦7 🟦7', 'freeze target');
      els.eventTitle.textContent = '🟦7を狙え';
      els.eventNote.textContent = '3停止でFREEZE確定';
    }, 1500));

    autoTimers.push(setTimeout(() => {
      els.machine.classList.remove('freeze-return');
      entryCinematicActive = false;
      for (let i = 0; i < 3; i++) {
        stops[i].disabled = false;
        stops[i].classList.add('active');
      }
      if (autoEnabled) {
        clearAutoTimers();
        [0,1,2].forEach((reelIndex, i) => {
          autoTimers.push(setTimeout(() => stopReelMotion(reelIndex), 650 + i * 350));
        });
      }
    }, 2250));
  };

  const stopReelMotion = (index) => {
    if (!gameActive || reelStopped[index] || entryCinematicActive) return;

    const isFirstStop = pendingPressedOrder.length === 0;
    if (isFirstStop && !pendingWasAT && !pendingWasBonus && !pendingWasChallenge && index !== 0) {
      // 通常時の変則押しペナルティ: 1回につき天井+1G。
      callJson('slot_apply_normal_penalty_json');
    }

    const isBellNavi = pendingBellNaviActive
      && pendingPhysicalRole === 'bell9'
      && pendingControl
      && pendingControl.navOrder >= 0
      && pendingControl.navOrder < stopOrders.length;
    const expectedIndex = isBellNavi ? pendingStopOrder[pendingPressedOrder.length] : -1;
    if (isBellNavi && index !== expectedIndex) pendingNaviOrderValid = false;
    pendingPressedOrder.push(index);
    const navigatedBell = isBellNavi && pendingNaviOrderValid && index === expectedIndex;
    const naviMiss = isBellNavi && !pendingNaviOrderValid;

    const pressedBase = pressedPositionAtPayline(index);

    if (reelTimers[index]) {
      clearInterval(reelTimers[index]);
      reelTimers[index] = null;
    }

    // 内部のtimer位相ではなく、押した瞬間にメイン有効ライン上にいた位置を基準にする。
    reelPositions[index] = pressedBase;
    const finalPosition = chooseStopPosition(index,navigatedBell,naviMiss,pressedBase);


    reelPositions[index] = finalPosition;
    reelStopped[index] = true;

    reels[index].classList.remove('spinning','reverse-spinning');
    renderReel(index);
    settleReelArt(index,pressedBase,finalPosition);
    stops[index].classList.remove('active','nav-first');
    stops[index].disabled = true;

    if (reelStopped.every(Boolean)) {
      if (pendingEntryAmbiguous) {
        if (pendingRole==='at') {
          showMachineCinematic('RESULT','AT 突入！','内部当選','gold judge-result');
        } else if (pendingRole==='hit') {
          showMachineCinematic('RESULT','BONUS！','内部当選','red judge-result');
        }
        autoTimers.push(setTimeout(hideMachineCinematic,650));
      }
      finishGame();
    }
  };

  const isPremium = (type) => ['freeze','upper_special_zone'].includes(type);
  const isHot = (type) => ['episode_bonus','at_start','special_zone','upper_comeback','section_cross'].includes(type);

  const pushEvents = (events) => {
    for (const e of events) {
      // BONUS / EPISODE の50/80は開始時の即時払出ではない。
      // 獲得予定枚数は bonusMedalsLeft で管理し、開始イベントには枚数を出さない。
      const value = (e.type === 'bonus' || e.type === 'episode_bonus') ? 0 : e.value;
      history.unshift({ ...e, value, at: new Date().toLocaleTimeString('ja-JP',{hour12:false}) });
    }
    history.splice(14);
    els.history.innerHTML = history.map(e =>
      '<li class="' + (isPremium(e.type) ? 'premium' : '') + '"><b>' +
      (eventLabels[e.type] || e.type) + '</b><span>' + (e.note || '') +
      (e.value ? ' [' + e.value + ']' : '') + '</span></li>'
    ).join('');
  };

  const showFinalBanner = (events, actualRole, payout) => {
    if (events.length) {
      const e = events[events.length - 1];
      const internalEvent = new Set([
        'bonus','episode_bonus','at_start','tier_up','freeze',
        'special_zone','upper_special_zone','upper_comeback','section_cross'
      ]).has(e.type);

      els.eventTitle.textContent = eventLabels[e.type] || e.type.toUpperCase();

      // 内部当選と停止役を因果関係のように1文へ混ぜない。
      // BONUS/ATは内部結果、下の「成立役」はそのゲームの物理停止役として別表示。
      els.eventNote.textContent = internalEvent
        ? ((e.note || '') + (e.note ? ' / ' : '') + '内部結果')
        : ((e.note || '') + ' / 停止役: ' + (roleLabels[actualRole] || actualRole)
            + (payout ? ' / ' + payout + '枚' : ''));

      els.eventBanner.className = 'event-banner'
        + (isPremium(e.type) ? ' premium' : isHot(e.type) ? ' hot' : '');
    } else {
      els.eventTitle.textContent = roleLabels[actualRole] || 'NO HIT';
      els.eventNote.textContent = payout ? payout + '枚払出' : '次ゲームへ';
      els.eventBanner.className = 'event-banner';
    }
  };

  const displayState = (raw) => {
    if (atOmenFlow) {
      return {
        ...raw,
        inAT: true,
        inBonus: false,
        episodeBonus: false,
        challengeActive: false,
        bonusMedalsLeft: 0
      };
    }
    if (!deferredEntryReveal) return raw;
    return {
      ...raw,
      inAT: false,
      inBonus: false,
      episodeBonus: false,
      challengeActive: false,
      atGamesLeft: 0,
      bonusMedalsLeft: 0
    };
  };

  const render = (s) => {
    const modeText = s.inBonus
      ? (s.episodeBonus ? 'EPISODE' : 'BONUS')
      : (s.challengeActive ? 'AT CHALLENGE'
          : (s.highProbabilityActive ? '高確率' : (labels[s.normalMode] || s.normalMode)));
    els.mode.textContent = modeText;
    els.tier.textContent = s.inAT ? (labels[s.atTier] || s.atTier) : '---';
    els.table.textContent = s.inAT ? (labels[s.atTable] || s.atTable) : '---';
    els.gameCount.textContent = s.totalGames.toLocaleString();
    els.atLeft.textContent = s.inBonus
      ? s.bonusMedalsLeft + '枚'
      : (s.challengeActive ? s.challengeGamesLeft + 'G' : (s.inAT ? s.atGamesLeft + 'G' : '0G'));
    els.stocks.textContent = s.stocks;
    els.normalPattern.textContent = 'P' + s.normalPattern;
    els.ceiling.textContent = s.challengeActive
      ? 'POINT ' + s.challengePoints + '/10 / 残り ' + s.challengeGamesLeft + 'G'
      : ('天井 ' + s.normalCeiling + 'G / 表示 ' + s.normalDisplayGames + 'G / 実 ' + s.normalActualGames + 'G');
    els.atPattern.textContent = s.inAT ? 'P' + s.atPattern : '-';
    els.netRate.textContent = s.inBonus
      ? '純増 約6枚/G'
      : (s.inAT ? '純増 約' + (s.atTier === 'upper' ? '9' : '6') + '枚/G' : '純増 -');
    els.totalDiff.textContent = (s.totalDiff >= 0 ? '+' : '') + s.totalDiff.toLocaleString();
    updateDiffPanel(s);
    els.debug.textContent = JSON.stringify(s, null, 2);
    const active = s.inAT || s.inBonus || s.challengeActive;
    els.statusLamp.className = 'status-lamp ' + (active ? 'at' : 'live');
    els.statusText.textContent = s.inBonus
      ? (s.episodeBonus ? 'EPISODE' : 'BONUS')
      : (s.challengeActive ? 'CHALLENGE' : (s.inAT ? 'AT' : 'NORMAL'));
    updateStageScene(s);
  };

  const beginGame = () => {
    if (gameActive) return;

    gameActive = true;
    pendingResult = null;
    reelStopped = [false, false, false];
    pendingPressedOrder = [];
    pendingNaviOrderValid = true;

    els.lever.disabled = true;
    els.roleResult.textContent = '回転中';
    els.payoutResult.textContent = '---';
    els.eventTitle.textContent = 'SPINNING';
    els.eventNote.textContent = 'STOPボタンでリールを止めろ';
    els.eventBanner.className = 'event-banner';

    const s0 = state();
    pendingWasBonus = !!s0.inBonus;
    pendingWasChallenge = !!s0.challengeActive;
    pendingWasAT = !!s0.inAT && !pendingWasBonus && !pendingWasChallenge;
    pendingBellNaviActive = false;
    pendingEntryReplayRole = false;
    pendingControl = null;
    pendingGuaranteedLines = null;
    pendingSyntheticEntry = false;
    pendingEntryAlignFlag = false;
    pendingSyntheticOmen = false;
    pendingEntryAmbiguous = false;
    clearBellNavi();

    // ここがレバーON抽選。内部AT/BONUS当選と物理リール停止役は別管理する。
    const forcedRole = els.roleTest.value;
    if (forcedRole) els.roleTest.value = '';

    // DEBUGのAT/当たりも、通常時なら「通常当選の行き先テスト」として扱う。
    // 内部結果だけ強制し、演出は本番と同じくAT/BONUSを最後まで隠す。
    const forcedNormalDestination = !!forcedRole
      && !pendingWasAT
      && !pendingWasBonus
      && !pendingWasChallenge
      && (forcedRole === 'hit' || forcedRole === 'at');

    pendingSyntheticEntry = false;
    pendingEntryAlignFlag = false;
    pendingSyntheticOmen = false;

    if (deferredEntryReveal) {
      // BONUS/ATの初回開始待ち。
      // 内部当選はすでに成立済みだが、対応図柄が実際に揃うまで開始表示へ進めない。
      // 各G 1/2で「図柄揃い許可フラグ」を立てる。
      const queued = deferredEntryReveal;
      pendingSyntheticEntry = true;
      pendingEntryAlignFlag = Math.random() < 0.5;
      pendingEntryAmbiguous = queued.ambiguous !== false;
      pendingWasBonus = false;
      pendingWasAT = false;
      pendingWasChallenge = false;

      pendingRole = queued.role; // 内部開始待ち: hit=BONUS / at=AT
      const waitPhysicalRole = pendingEntryAlignFlag
        ? queued.role
        : chooseEntryWaitPhysicalRole();

      pendingResult = {
        events: [],
        inAT: false,
        inBonus: false,
        navOrder: -1,
        // 1/2フラグOFF時は普通に遊技する。
        // ON時だけ開始図柄フラグを停止制御へ渡す。
        reelRole: waitPhysicalRole,
        reelPayout: accountingReturnForRole(waitPhysicalRole)
      };
    } else if (atOmenFlow && atOmenFlow.phase === 'omen') {
      // 旧WASMはすでにBONUSへ遷移済みなので、WASMを進めずAT予兆Gを挟む。
      pendingSyntheticOmen = true;
      pendingWasBonus = false;
      pendingWasAT = true;
      pendingWasChallenge = false;
      const omenNav = Math.floor(Math.random() * 6);
      pendingResult = {
        events: [{ type:'at_omen', value:0, note:'AT予兆' }],
        inAT: true,
        inBonus: false,
        navOrder: omenNav,
        reelRole: 'bell9',
        reelPayout: 0
      };
      pendingRole = 'bell9';
      atOmenFlow.phase = 'entry';
    } else {
      pendingResult = forcedRole
        ? callJson('slot_force_outcome_json', ['number'], [forceOutcomeCodes[forcedRole]])
        : callJson(s0.inBonus
            ? 'slot_spin_bonus_json'
            : (s0.challengeActive
                ? 'slot_spin_challenge_json'
                : (s0.inAT ? 'slot_spin_at_json' : 'slot_spin_normal_json')));
      pendingRole = deriveRoleFromResult(pendingResult, pendingWasChallenge);

      if (forcedNormalDestination) {
        pendingEntryAmbiguous = true;
      }

      // 旧WASMのAT当たりは同GでBONUS/EPISODEへ直行するため、
      // その遷移を捕まえて「予兆1G → 当たり入賞G」に分解する。
      const atDirectHit = pendingWasAT
        && !forcedRole
        && !!pendingResult.inBonus;
      if (atDirectHit) {
        const entryTypes = new Set(['bonus','episode_bonus']);
        const entryEvents = (pendingResult.events || []).filter(e => entryTypes.has(e.type));
        const episode = !!pendingResult.episodeBonus
          || entryEvents.some(e => e.type === 'episode_bonus');

        atOmenFlow = {
          phase: 'omen',
          episode,
          events: entryEvents.length
            ? entryEvents
            : [{ type: episode ? 'episode_bonus' : 'bonus', value:0, note:'AT予兆 → 当たり' }]
        };

        // 当選したATゲーム自体はまだATとして見せる。BONUS開始表示は出さない。
        const physicalTriggerRole = pendingResult.reelRole || 'bell9';
        pendingRole = physicalTriggerRole;
        pendingResult = {
          ...pendingResult,
          inAT: true,
          inBonus: false,
          episodeBonus: false,
          reelRole: physicalTriggerRole,
          events: [
            ...(pendingResult.events || []).filter(e => !entryTypes.has(e.type)),
            { type:'at_omen', value:1, note:'AT内部当選 → 予兆開始' }
          ]
        };
      }

      // 通常時の内部AT/BONUS当選はレバーONで確定するが、
      // リール側はそのゲームで実際に成立した物理役だけを停止させる。
      // 次ゲームへ「リプレイ入賞ゲーム」を捏造しない。
      const normalTrigger = !pendingWasAT && !pendingWasBonus && !pendingWasChallenge;
      const physicalTriggerRole = pendingResult.reelRole || 'miss';
      const naturalTransition = normalTrigger
        && (!forcedRole || forcedNormalDestination)
        && (pendingResult.inBonus || pendingResult.inAT);

      if (naturalTransition) {
        const entryEvents = (pendingResult.events || []).filter(e =>
          ['bonus','episode_bonus','at_start','cold_enter','stock_gain','tier_up','freeze'].includes(e.type)
        );
        const hasFreezeEntry = entryEvents.some(e => e.type === 'freeze');

        // STOP完了までは表示上だけNORMALに伏せる。
        // finishGameでこのマーカーを解除すると、WASM内部のBONUS/AT状態をそのまま表示する。
        deferredEntryReveal = {
          role: hasFreezeEntry ? 'freeze' : (pendingResult.inBonus ? 'hit' : 'at'),
          events: entryEvents,
          ambiguous: !hasFreezeEntry
        };

        pendingEntryAmbiguous = !hasFreezeEntry;
        pendingRole = physicalTriggerRole;
        pendingResult = {
          ...pendingResult,
          reelRole: physicalTriggerRole,
          // 内部開始イベントは図柄が揃った瞬間まで画面へ出さない。
          events: (pendingResult.events || []).filter(e =>
            !['bonus','episode_bonus','at_start','cold_enter','stock_gain','tier_up','freeze'].includes(e.type)
          )
        };
      }
    }

    // 現行WASMはBONUSの内部差枚を+6/Gで管理する。
    // 画面上の成立役は非レアGを「減らないBONUS専用分布」へ固定する。
    if (pendingWasBonus && !forcedRole && !bonusRareRoles.has(pendingRole)) {
      pendingRole = chooseBonusSafeRole();
      pendingResult = {
        ...pendingResult,
        reelRole: pendingRole,
        navOrder: pendingRole === 'bell9'
          ? (Number.isInteger(Number(pendingResult.navOrder)) && Number(pendingResult.navOrder) >= 0
              ? Number(pendingResult.navOrder)
              : Math.floor(Math.random() * 6))
          : -1
      };
    }

    pendingPhysicalRole = pendingResult.reelRole || pendingRole;
    pendingEntryReplayRole = pendingPhysicalRole === 'replay';

    pendingPayout = pendingWasChallenge
      ? 0
      : (pendingWasBonus
          ? bonusVisibleReturnForRole(pendingPhysicalRole)
          : (forcedRole
              ? accountingReturnForRole(pendingPhysicalRole)
              : Number(pendingResult.reelPayout ?? accountingReturnForRole(pendingPhysicalRole))));

    // レバーONで内部結果(pendingRole)はここまでに確定済み。
    // リール停止制御は別物。AT/BONUSの内部当選を赤7/BARへ強制変換せず、
    // そのゲームで実際に成立している物理役(reelRole)だけを0〜4コマ制御する。
    pendingControl = buildSpinControl(pendingPhysicalRole, pendingResult);
    pendingGuaranteedLines = guaranteedPayoutRoles.has(pendingPhysicalRole)
      ? guaranteedLinesForRole(pendingPhysicalRole)
      : null;
    pendingBellNaviActive = (pendingWasAT || pendingWasBonus)
      && pendingPhysicalRole === 'bell9'
      && Number.isInteger(pendingControl.navOrder)
      && pendingControl.navOrder >= 0
      && pendingControl.navOrder < stopOrders.length;

    if (pendingWasAT) {
      pendingPayout = 0; // AT純増はWASM側で既に差枚会計済み。
    }
    if (pendingBellNaviActive) {
      setBellNavi(pendingControl.navOrder);
    }

    stageCue(
      pendingSyntheticEntry
        ? (pendingEntryAlignFlag ? 'entry_judge' : pendingPhysicalRole)
        : pendingPhysicalRole,
      'spin'
    );

    if ((pendingSyntheticEntry || forcedRole === 'freeze') && pendingRole === 'freeze') {
      clearAutoTimers();
      runFreezeEntryCinematic();
      return;
    }

    if (pendingSyntheticEntry && (pendingRole === 'hit' || pendingRole === 'at')) {
      clearAutoTimers();

      if (!pendingEntryAlignFlag) {
        // 1/2フラグOFFは完全に通常遊技として見せる。
        for (let i = 0; i < 3; i++) startReelMotion(i);

        if (autoEnabled) {
          clearAutoTimers();
          [0,1,2].forEach((reelIndex,i) => {
            autoTimers.push(setTimeout(() => stopReelMotion(reelIndex), 450 + i * 250));
          });
        }
        return;
      }

      // 1/2フラグONでも、いきなり「7を狙え」は出さない。
      // まず当たり告知 → 少し間を置いて狙え、の順にする。
      entryCinematicActive = true;
      for (let i = 0; i < 3; i++) {
        startReelMotion(i);
        stops[i].disabled = true;
        stops[i].classList.remove('active');
      }

      const queued = deferredEntryReveal;
      const ambiguous = queued?.ambiguous !== false;

      showMachineCinematic(
        'HIT',
        ambiguous ? '当たり' : (pendingRole === 'at' ? 'AT 確定' : 'BONUS 確定'),
        ambiguous ? '行き先はまだ分からない' : '',
        ambiguous ? 'judge' : (pendingRole === 'at' ? 'gold' : 'red')
      );
      els.eventTitle.textContent = ambiguous ? '当たり' : '確定';
      els.eventNote.textContent = ambiguous ? '図柄告知待機' : '開始図柄を狙え';

      autoTimers.push(setTimeout(() => {
        showMachineCinematic(
          'TARGET',
          '🟥7を狙え',
          ambiguous ? '右リールの 🟥7 / BAR で行き先決定' : '押した位置から最大4コマ引き込み',
          pendingRole === 'at' ? 'gold target' : 'red target'
        );
        els.eventTitle.textContent = '🟥7を狙え';
        els.eventNote.textContent = ambiguous
          ? '右リールでAT / BONUSをジャッジ'
          : '押した位置から最大4コマ引き込み';

        entryCinematicActive = false;
        for (let i = 0; i < 3; i++) {
          stops[i].disabled = false;
          stops[i].classList.add('active');
        }

        if (autoEnabled) {
          clearAutoTimers();
          [0,1,2].forEach((reelIndex,i) => {
            autoTimers.push(setTimeout(() => stopReelMotion(reelIndex), 500 + i * 280));
          });
        }
      }, 600));

      return;
    }

    for (let i = 0; i < 3; i++) startReelMotion(i);

    if (autoEnabled) {
      clearAutoTimers();
      const order = pendingBellNaviActive ? pendingStopOrder : [0,1,2];
      order.forEach((reelIndex, i) => {
        autoTimers.push(setTimeout(() => stopReelMotion(reelIndex), 450 + i * 250));
      });
    }
  };

  const finishGame = () => {
    if (!gameActive) return;

    gameActive = false;
    entryCinematicActive = false;
    if (freezeSequenceActive) {
      freezeSequenceActive = false;
      els.machine.classList.remove('freeze-all-off','freeze-return');
    }
    hideMachineCinematic();
    els.lever.disabled = false;

    const result = pendingResult || { events: [] };
    pendingResult = null;

    // 停止形は結果ではなく表示。成立役はレバーON時のpendingRoleが唯一の結果。
    const physicalPattern = classifyPattern();
    const assistSubstitute = !!pendingControl?.substituteUsed?.some(Boolean);
    const aimAssistUsed = !!pendingControl?.aimAssistUsed?.some(Boolean);
    const naviMiss = pendingBellNaviActive
      && pendingPhysicalRole === 'bell9'
      && pendingControl
      && pendingControl.navOrder >= 0
      && !pendingNaviOrderValid;
    const payout = pendingPayout;
    const physicalLinePayoutRole = classifyPhysicalLinePayout();

    // ベル/リプレイは「内部予約役」より実停止ラインを優先する。
    // 5ラインのどこかで揃っていれば、その停止役が有効。
    const guaranteedRoleExpected = guaranteedPayoutRoles.has(pendingPhysicalRole);
    const guaranteedPhysicalRole = guaranteedRoleExpected
      ? physicalLinePayoutRole
      : null;

    const cherryPhysical = pendingPhysicalRole === 'weak_cherry'
      || pendingPhysicalRole === 'strong_cherry';

    const visibleRole = pendingSyntheticEntry
      ? physicalPattern
      : (cherryPhysical
          ? physicalPattern
          : (naviMiss
              ? physicalPattern
              : (guaranteedRoleExpected
                  ? (guaranteedPhysicalRole || physicalPattern)
                  : (physicalLinePayoutRole || pendingPhysicalRole))));

    const visiblePayout = guaranteedRoleExpected
      ? (guaranteedPhysicalRole
          ? accountingReturnForRole(guaranteedPhysicalRole)
          : 0)
      : (physicalLinePayoutRole
          ? accountingReturnForRole(physicalLinePayoutRole)
          : (naviMiss ? accountingReturnForRole(physicalPattern) : payout));
    let allEvents = [...(result.events || [])];

    if (pendingSyntheticOmen && atOmenFlow && atOmenFlow.phase === 'entry') {
      // AT中の予兆明けも同じ開始待ちへ接続する。
      deferredEntryReveal = {
        role: 'hit',
        events: [...(atOmenFlow.events || [])],
        ambiguous: false
      };
      atOmenFlow = null;
    }

    if (pendingSyntheticEntry && deferredEntryReveal) {
      const queued = deferredEntryReveal;
      const entrySucceeded = pendingEntryAlignFlag
        && physicalPattern === queued.role;

      if (entrySucceeded) {
        // 対応図柄が実際に揃ったこの瞬間にだけBONUS/AT開始イベントを公開。
        allEvents = allEvents.concat(queued.events || []);
        deferredEntryReveal = null;
        pendingEntryAmbiguous = false;

        showMachineCinematic(
          'START',
          queued.role === 'at' ? 'AT START' : 'BONUS START',
          queued.role === 'at' ? '🟥7 🟥7 🟥7' : '🟥7 🟥7 BAR',
          queued.role === 'at' ? 'gold judge-result' : 'red judge-result'
        );
        autoTimers.push(setTimeout(hideMachineCinematic,700));
      } else {
        // 内部当選は保持。次Gでもう一度1/2抽選から開始。
        allEvents = [];
      }
    }

    // 通常時は、実停止5ラインで新たに成立したベル/リプレイも差枚へ反映。
    // AT/BONUSは内部純増会計済みなので二重加算しない。
    const accountingPayout = (!pendingWasAT && !pendingWasBonus && physicalLinePayoutRole)
      ? accountingReturnForRole(physicalLinePayoutRole)
      : payout;
    if (accountingPayout > 0 && !pendingWasBonus && !pendingWasAT && !pendingSyntheticEntry && !pendingSyntheticOmen) {
      const payoutResult = callJson('slot_apply_reel_payout_json', ['number'], [accountingPayout]);
      allEvents = allEvents.concat(payoutResult.events || []);
    }

    els.roleResult.textContent = pendingSyntheticEntry || cherryPhysical
      ? (roleLabels[physicalPattern] || physicalPattern)
      : (naviMiss
          ? 'ナビ外し / ' + (roleLabels[physicalPattern] || physicalPattern)
          : (guaranteedRoleExpected
              ? (guaranteedPhysicalRole
                  ? (pendingBellNaviActive
                      ? '🔔 押し順ベル'
                      : (roleLabels[guaranteedPhysicalRole] || guaranteedPhysicalRole))
                  : (roleLabels[physicalPattern] || physicalPattern))
              : (physicalLinePayoutRole
                  ? (roleLabels[physicalLinePayoutRole] || physicalLinePayoutRole)
                  : (roleLabels[pendingPhysicalRole] || pendingPhysicalRole))));
    els.payoutResult.textContent = pendingWasChallenge
      ? ('POINT ' + Number(result.challengePoints ?? state().challengePoints) + '/10')
      : (visibleRole === 'replay'
          ? 'REPLAY'
          : (naviMiss
              ? visiblePayout + '枚'
              : (pendingWasBonus
                  ? '+' + visiblePayout + '枚'
                  : (pendingBellNaviActive && pendingWasAT
                      ? 'AT純増'
                      : (visibleRole === 'replay' ? 'REPLAY' : visiblePayout + '枚')))));

    pushEvents(allEvents);
    showFinalBanner(allEvents, visibleRole, visiblePayout);

    if (guaranteedRoleExpected && !guaranteedPhysicalRole) {
      els.eventNote.textContent = '成立ライン不成立 / 払出なし';
    }

    if (pendingSyntheticEntry && deferredEntryReveal && pendingEntryAlignFlag) {
      // 「狙え」を出したGだけ、失敗時に次Gへ持ち越すことを表示。
      // フラグOFFの通常遊技では、強チェ等の成立役表示を絶対に上書きしない。
      els.eventTitle.textContent = '図柄揃わず';
      els.eventNote.textContent = '次ゲームへ';
    }
    if (naviMiss) {
      els.eventNote.textContent += ' / ナビ外し（内部成立: 押し順ベル）';
    }
    if (assistSubstitute) {
      els.eventNote.textContent += ' / 代用停止';
    }
    if (aimAssistUsed) {
      els.eventNote.textContent += ' / 目押しアシスト';
    }
    if (!pendingSyntheticEntry
        && !cherryPhysical
        && !naviMiss && !physicalLinePayoutRole
        && physicalPattern !== pendingPhysicalRole
        && !assistSubstitute
        && !aimAssistUsed
        && !pendingControl?.exactAssistUsed?.some(Boolean)
        && pendingPhysicalRole !== 'one_medal') {
      els.eventNote.textContent += ' / 取りこぼし停止';
    }
    stageCue(visibleRole, 'result');
    if (visibleRole === 'freeze') {
      showMachineCinematic('PREMIUM', 'FREEZE 確定', '🟦7 🟦7 🟦7', 'freeze confirmed');
      autoTimers.push(setTimeout(hideMachineCinematic, 900));
    }
    render(displayState(state()));
    if (!autoEnabled) clearBellNavi();

    if (autoEnabled) {
      clearAutoTimers();
      autoTimers.push(setTimeout(beginGame, 450));
    }
  };

  const setAuto = (enabled) => {
    autoEnabled = enabled;
    els.autoToggle.textContent = enabled ? 'AUTO ON' : 'AUTO OFF';
    els.autoToggle.classList.toggle('on', enabled);
    els.autoToggle.setAttribute('aria-pressed', enabled ? 'true' : 'false');

    if (!enabled) {
      clearAutoTimers();
      return;
    }

    if (!gameActive) {
      clearAutoTimers();
      autoTimers.push(setTimeout(beginGame, 250));
    } else {
      clearAutoTimers();
      const order = pendingBellNaviActive ? pendingStopOrder : [0,1,2];
      let delay = 250;
      for (const reelIndex of order) {
        if (!reelStopped[reelIndex]) {
          autoTimers.push(setTimeout(() => stopReelMotion(reelIndex), delay));
          delay += 250;
        }
      }
    }
  };

  const resetMachine = () => {
    clearAutoTimers();
    autoEnabled = false;
    els.autoToggle.textContent = 'AUTO OFF';
    els.autoToggle.classList.remove('on');
    els.autoToggle.setAttribute('aria-pressed', 'false');

    reelTimers.forEach((timer, i) => {
      if (timer) clearInterval(timer);
      reelTimers[i] = null;
      cancelReelArtFrame(i);
      reels[i].classList.remove('spinning','reverse-spinning');
      stops[i].classList.remove('active');
      stops[i].disabled = true;
    });

    gameActive = false;
    pendingResult = null;
    pendingRole = 'miss';
    pendingPhysicalRole = 'miss';
    pendingPayout = 0;
    pendingWasAT = false;
    pendingWasBonus = false;
    pendingWasChallenge = false;
    pendingBellNaviActive = false;
    pendingEntryReplayRole = false;
    pendingPressedOrder = [];
    pendingNaviOrderValid = true;
    pendingControl = null;
    pendingGuaranteedLines = null;
    pendingSyntheticEntry = false;
    pendingEntryAlignFlag = false;
    pendingSyntheticOmen = false;
    entryCinematicActive = false;
    freezeSequenceActive = false;
    pendingEntryAmbiguous = false;
    deferredEntryReveal = null;
    atOmenFlow = null;
    els.machine.classList.remove('freeze-all-off','freeze-return');
    hideMachineCinematic();
    clearBellNavi();
    reelStopped = [true, true, true];
    els.lever.disabled = false;

    const seed = BigInt(Date.now());
    Module.ccall('slot_reset', null, ['number','number'], [
      Number(seed & 0xffffffffn),
      Number(seed >> 32n)
    ]);

    history.length = 0;
    els.history.innerHTML = '';
    diffHistory.splice(0, diffHistory.length, { game:0, diff:0 });
    diffLastGame = 0;
    els.roleResult.textContent = '---';
    els.payoutResult.textContent = '0枚';
    els.eventTitle.textContent = 'RESET';
    els.eventNote.textContent = 'レバーを叩け';
    els.eventBanner.className = 'event-banner';
    clearStageClasses();
    els.cutinLayer.classList.remove('show');
    els.stageCaption.textContent = '通常ステージ';
    render(state());
  };

  els.lever.onclick = beginGame;
  stops.forEach((button, index) => {
    button.disabled = true;
    button.onclick = () => stopReelMotion(index);
  });
  els.autoToggle.onclick = () => setAuto(!autoEnabled);
  els.reset.onclick = resetMachine;

  reelPositions = reelStrips.map(strip => Math.floor(Math.random() * strip.length));
  reelPositions.forEach((_, i) => renderReel(i));
  render(state());
})();