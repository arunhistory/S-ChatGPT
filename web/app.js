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

  // 確定済み21コマ配列
  // 🍉=2/2/1, 🍒=2/1/0, 🔔=2/2/2
  // 右11〜13は 青7→赤7→BAR
  const reelStrips = [
    ['seven','replay','miss','cherry','bar','replay','bell','penguin','miss','alt-seven','replay','watermelon','cherry','bar','miss','replay','bell','chance','watermelon','miss','replay'],
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
  let deferredEntryReveal = null;
  let atOmenFlow = null;
  let pendingSyntheticEntry = false;
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
    if (!source) return;

    const src = source.getAttribute('src');
    if (!src) return;

    // 21コマを1コマずつDOM化し、4周分だけ並べる。
    // 元画像そのものをbackgroundとして使うためcanvas変換は不要。
    // 各周は逆順にすることで、ベルトを下へ動かすと内部positionの+方向と一致する。
    const fragment = document.createDocumentFragment();
    for (let copy = 0; copy < 4; copy++) {
      for (let visualCell = 0; visualCell < 21; visualCell++) {
        const logicalCell = mod(-visualCell, 21);
        const cell = document.createElement('div');
        cell.className = 'reel-art-cell';
        cell.dataset.logical = String(logicalCell);
        cell.style.backgroundImage = 'url("' + src + '")';
        fragment.appendChild(cell);
      }
    }

    track.innerHTML = '';
    track.appendChild(fragment);
    reelArtReady[index] = true;
  };

  const bootReelArtIfNeeded = (index) => {
    prepareReelBelt(index);
    if (!reelArtReady[index]) return;

    renderReelArt(index, true);

    // レバーONが画像ロード完了より先でも、ロード完了した瞬間に画像回転を追従開始。
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

    const reelHeight = reels[index].clientHeight;
    if (!reelHeight) return null;

    const cellHeight = reelHeight / 3;
    const stripHeight = cellHeight * 21;

    // 上下端の丸い「リール終端」は使わない。
    // 元画像の約1.2%ずつを上下から除外した21コマ領域だけを各セルへ投影。
    const visibleFraction = 0.976;
    const bgHeight = stripHeight / visibleFraction;
    const topTrim = bgHeight * 0.012;

    const cells = track.querySelectorAll('.reel-art-cell');
    cells.forEach((cell) => {
      const logicalCell = Number(cell.dataset.logical || 0);
      cell.style.height = cellHeight + 'px';
      cell.style.backgroundSize = 'auto ' + bgHeight + 'px';
      cell.style.backgroundPosition =
        'center ' + (-(topTrim + logicalCell * cellHeight)) + 'px';
    });

    return { track, cellHeight, stripHeight };
  };

  const targetReelArtY = (index) => {
    const geo = reelArtGeometry(index);
    if (!geo) return null;

    // 上段=position-1 が画面上端へ来るように、中央寄りのコピーを使う。
    const topLogical = mod(reelPositions[index] - 1, 21);
    const visualIndex = mod(-topLogical, 21);
    const cellIndex = 21 + visualIndex;
    return -(cellIndex * geo.cellHeight);
  };

  const paintReelArt = (index) => {
    const geo = reelArtGeometry(index);
    if (!geo) return;
    geo.track.style.transform =
      'translate3d(0,' + reelArtPhase[index] + 'px,0)';
  };

  const renderReelArt = (index, immediate = false) => {
    // 回転中はrequestAnimationFrameが連続描画を担当する。
    if (reelArtFrames[index] !== null && !immediate) return;

    const y = targetReelArtY(index);
    if (y === null) return;

    reelArtPhase[index] = y;
    paintReelArt(index);
  };

  const cancelReelArtFrame = (index) => {
    if (reelArtFrames[index] !== null) {
      cancelAnimationFrame(reelArtFrames[index]);
      reelArtFrames[index] = null;
    }
  };

  const normalizeReelArtY = (index) => {
    const geo = reelArtGeometry(index);
    if (!geo) return;

    const cycle = geo.stripHeight;

    // 4周の中央2周だけを使う。1周分の補正は見た目が完全に同じなので飛ばない。
    while (reelArtPhase[index] > -cycle) reelArtPhase[index] -= cycle;
    while (reelArtPhase[index] < -3 * cycle) reelArtPhase[index] += cycle;
  };

  const startReelArtMotion = (index, direction) => {
    cancelReelArtFrame(index);

    const geo = reelArtGeometry(index);
    if (!geo) return;

    reelArtDirection[index] = direction;

    if (!Number.isFinite(reelArtPhase[index])) {
      reelArtPhase[index] = targetReelArtY(index) ?? -geo.stripHeight;
    }

    // 通常回転=約8.7コマ/秒。逆回転だけ少し速め。
    const secondsPerCell = direction > 0 ? 0.115 : 0.090;
    const speed = geo.cellHeight / secondsPerCell;
    let last = performance.now();

    const frame = (now) => {
      const currentGeo = reelArtGeometry(index);
      if (!currentGeo) {
        reelArtFrames[index] = requestAnimationFrame(frame);
        return;
      }

      const dt = Math.min(0.035, Math.max(0, (now - last) / 1000));
      last = now;

      // 通常はひたすら下向き。逆回転時だけ上向き。
      reelArtPhase[index] += direction * speed * dt;
      normalizeReelArtY(index);
      paintReelArt(index);

      reelArtFrames[index] = requestAnimationFrame(frame);
    };

    reelArtFrames[index] = requestAnimationFrame(frame);
  };

  const settleReelArt = (index) => {
    cancelReelArtFrame(index);

    const geo = reelArtGeometry(index);
    if (!geo) return;

    const base = targetReelArtY(index);
    if (base === null) return;

    const cycle = geo.stripHeight;
    const current = reelArtPhase[index];

    // 同じ停止位置は1周ごとに無限に存在する。
    // 今流れている方向を維持したまま、次に来る同一位置へ着地させる。
    let target = base;
    if (reelArtDirection[index] >= 0) {
      while (target < current) target += cycle;
    } else {
      while (target > current) target -= cycle;
    }

    const distance = Math.abs(target - current);
    const duration = Math.max(90, Math.min(320, 85 + (distance / geo.cellHeight) * 28));
    const startY = current;
    const started = performance.now();
    const easeOut = (t) => 1 - Math.pow(1 - t, 3);

    const frame = (now) => {
      const t = Math.min(1, (now - started) / duration);
      reelArtPhase[index] = startY + (target - startY) * easeOut(t);
      paintReelArt(index);

      if (t < 1) {
        reelArtFrames[index] = requestAnimationFrame(frame);
      } else {
        reelArtFrames[index] = null;
        reelArtPhase[index] = target;
        normalizeReelArtY(index);
        paintReelArt(index);
      }
    };

    reelArtFrames[index] = requestAnimationFrame(frame);
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

    const leftBottom = visibleKind(0, positions[0], 2);
    if (leftBottom === 'cherry' && center[1] === 'replay') return 'weak_cherry';
    if (center[0] === 'cherry' && center[1] !== 'replay') return 'strong_cherry'; // 中段チェリー

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
      // 通常当選の左・中は共通で🟥7。右リールだけがAT=🟥7 / 当たり=BAR。
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

  // 代用停止は「成立したチャンス系役を、目押し失敗で取りこぼさない」ためだけに使う。
  // ベル／リプレイは実停止ラインで判定するため、ここには絶対に入れない。
  const assistSubstituteRoles = new Set([
    'watermelon','weak_chance','strong_chance','penguin_chance'
  ]);

  // 🟥7当選系は、STOP入力そのものを狙える位相へ補正する目押しアシスト。
  // 青7フリーズのような無条件強制停止とは分け、補正後も0〜4コマ引き込みで停止させる。
  const aimAssistRoles = new Set(['at','hit']);

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
      assistSubstitute: assistSubstituteRoles.has(role),
      substituteUsed: [false, false, false],
      aimAssist: aimAssistRoles.has(role),
      aimAssistUsed: [false, false, false],
      navOrder: Number(result.navOrder ?? -1)
    };
  };

  const chooseAssistSubstitutePosition = (index, base) => {
    // 成立役はレバーONで既に確定済み。ここでは停止表示だけを代用形へ落とす。
    const safeKinds = ['chance','bar','miss'];
    for (const kind of safeKinds) {
      for (let slip = 0; slip <= 4; slip++) {
        const candidate = mod(base + slip, reelStrips[index].length);
        if (visibleKind(index, candidate, 1) === kind) {
          pendingControl.substituteUsed[index] = true;
          return candidate;
        }
      }
    }
    pendingControl.substituteUsed[index] = true;
    return base;
  };

  const guaranteedPayoutRoles = new Set(['bell9','bell15','three_medal','replay']);

  const chooseGuaranteedPayoutPosition = (index, target, base) => {
    const strip = reelStrips[index];

    // ベル/REPLAY成立時は代用停止なし。
    // まず実機同様に0〜4コマで引き込み、それで届かなければ
    // 成立役制御として正規図柄位置まで収束させる。
    for (let slip = 0; slip <= 4; slip++) {
      const candidate = mod(base + slip, strip.length);
      if (candidateMatchesTarget(index, candidate, target)) return candidate;
    }

    for (let advance = 5; advance < strip.length + 5; advance++) {
      const candidate = mod(base + advance, strip.length);
      if (candidateMatchesTarget(index, candidate, target)) return candidate;
    }

    // リール配列破損時だけ現在位置を維持。正常配列では到達しない。
    return base;
  };

  const chooseAimAssistedPosition = (index, target, base) => {
    const strip = reelStrips[index];

    // 押した瞬間に狙い図柄が4コマ圏外なら、目押しアシストで有効STOP位相を少し先へ送る。
    // その有効位相からの停止自体は必ず0〜4コマの範囲に収める。
    for (let advance = 1; advance < strip.length; advance++) {
      const assistedBase = mod(base + advance, strip.length);
      for (let slip = 0; slip <= 4; slip++) {
        const candidate = mod(assistedBase + slip, strip.length);
        if (candidateMatchesTarget(index, candidate, target)) {
          pendingControl.aimAssistUsed[index] = true;
          return candidate;
        }
      }
    }

    return base;
  };

  const chooseNavigatedBellPosition = (index, base) => {
    const target = { row:1, kind:'bell' };
    const strip = reelStrips[index];

    // AT押し順ナビに正しく従った停止は取りこぼし不可。
    // まず通常の0〜4コマ引き込み、それで届かなければ有効位相を補正して
    // 必ず中段ベルへ着地させる。成立/純増はWASM側でレバーON時に確定済み。
    for (let slip = 0; slip <= 4; slip++) {
      const candidate = mod(base + slip, strip.length);
      if (candidateMatchesTarget(index, candidate, target)) return candidate;
    }

    for (let advance = 1; advance < strip.length; advance++) {
      const assistedBase = mod(base + advance, strip.length);
      for (let slip = 0; slip <= 4; slip++) {
        const candidate = mod(assistedBase + slip, strip.length);
        if (candidateMatchesTarget(index, candidate, target)) return candidate;
      }
    }

    // リール配列にベルが存在する限りここには来ないが、壊れた配列でも
    // 他役へ誤停止させず現在位置を維持する。
    return base;
  };

  const middleCherryMiddleReelSafe = (position) => {
    // 中段チェリー成立Gは、中リールの可視3コマすべてからREPLAYを排除する。
    // 5本の有効払出ラインは必ず中リールの上/中/下のいずれかを通るため、
    // これでリプレイ揃いそのものを物理的に不可能にする。
    return [0,1,2].every(row => visibleKind(1, position, row) !== 'replay');
  };

  const chooseMiddleCherryPosition = (index, base) => {
    const strip = reelStrips[index];

    if (index === 0) {
      // レバーON時点で中段チェリー成立済み。左中段🍒は取りこぼさせない。
      for (let slip = 0; slip <= 4; slip++) {
        const candidate = mod(base + slip, strip.length);
        if (visibleKind(0, candidate, 1) === 'cherry') return candidate;
      }
      // 4コマ圏外でも成立役制御として中段🍒位置へ収束させる。
      return findExactPosition(0, 'cherry', 1);
    }

    if (index === 1) {
      // 中リールは上中下すべてREPLAY禁止。
      for (let slip = 0; slip <= 4; slip++) {
        const candidate = mod(base + slip, strip.length);
        if (middleCherryMiddleReelSafe(candidate)) return candidate;
      }
      // 4コマ圏内に無ければ、成立役制御として最寄りの安全窓へ収束。
      for (let advance = 1; advance < strip.length; advance++) {
        const candidate = mod(base + advance, strip.length);
        if (middleCherryMiddleReelSafe(candidate)) return candidate;
      }
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
    const strip = reelStrips[index];

    // ハズレ成立Gは1停止目から安全目へ制御する。
    // まず実機的な0〜4コマ範囲で候補を探す。
    for (let slip = 0; slip <= 4; slip++) {
      const candidate = mod(base + slip, strip.length);
      if (!missCenterSafe(index, candidate)) continue;

      const test = [...reelPositions];
      test[index] = candidate;

      // 最終停止なら成立役っぽい完成形・ベル/REPLAY払出まで完全に排除。
      const willAllStop = reelStopped.filter(Boolean).length === 2;
      if (willAllStop && classifyPattern(test) !== 'miss') continue;

      return candidate;
    }

    // 4コマ内に安全位置が無い場合も、ハズレなのに煽り目を出すより安全目を優先する。
    for (let advance = 5; advance < strip.length + 5; advance++) {
      const candidate = mod(base + advance, strip.length);
      if (!missCenterSafe(index, candidate)) continue;

      const test = [...reelPositions];
      test[index] = candidate;
      const willAllStop = reelStopped.filter(Boolean).length === 2;
      if (willAllStop && classifyPattern(test) !== 'miss') continue;

      return candidate;
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

  const chooseStopPosition = (index, navigatedBell = false, naviMiss = false) => {
    const strip = reelStrips[index];
    const base = mod(reelPositions[index], strip.length);

    if (navigatedBell) {
      return chooseNavigatedBellPosition(index, base);
    }

    // AT押し順を外した後は、内部成立ベルの引き込み制御を使わない。
    // 実際に押したタイミングの停止形をそのまま「外した役」として扱う。
    if (naviMiss) {
      return base;
    }

    // 青7フリーズのみ0〜4コマ制御の外。押下位置を無視して中段へ強制揃い。
    if (!pendingControl) return base;

    if (pendingControl.role === 'freeze') {
      return findExactPosition(index, 'alt-seven', 1);
    }

    if (pendingEntryAmbiguous && index === 2
        && (pendingControl.role === 'at' || pendingControl.role === 'hit')) {
      return chooseAmbiguousRightJudgePosition(pendingControl.role);
    }

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

      for (let slip = 0; slip <= 4; slip++) {
        const candidate = mod(base + slip, strip.length);
        if (candidateMatchesTarget(index, candidate, target)) return candidate;
      }

      // 🟥7当選系は取りこぼしにせず、狙える位相までSTOPをアシストして成立ラインを完成させる。
      if (pendingControl.aimAssist) {
        return chooseAimAssistedPosition(index, target, base);
      }

      // アシスト対象役は4コマで本来図柄を引き込めなくても代用停止で成立を維持する。
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
        const candidate = mod(base + slip, strip.length);
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
      reelPositions[index] = mod(reelPositions[index] + 1, reelStrips[index].length);
      renderReel(index);
    }, 115);
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
      reelPositions[index] = mod(reelPositions[index] - 1, reelStrips[index].length);
      renderReel(index);
    }, 90);
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
      showMachineCinematic('TARGET', '🟥7を狙え', '左・中は🟥7　右は…？', 'judge target');
      els.stageScreen.classList.add('omen-purple');
      els.effectLayer.className = 'effect-layer purple';
      els.eventTitle.textContent = '🟥7を狙え';
      els.eventNote.textContent = '右リールの 🟥7 / BAR で行き先決定';
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
        '🟥7を狙え',
        isAT ? '🟥7 🟥7 🟥7' : '🟥7 🟥7 BAR',
        isAT ? 'gold target' : 'red target'
      );
      els.eventTitle.textContent = '🟥7を狙え';
      els.eventNote.textContent = 'STOPボタンで図柄を揃えろ';
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
      && pendingRole === 'bell9'
      && pendingControl
      && pendingControl.navOrder >= 0
      && pendingControl.navOrder < stopOrders.length;
    const expectedIndex = isBellNavi ? pendingStopOrder[pendingPressedOrder.length] : -1;
    if (isBellNavi && index !== expectedIndex) pendingNaviOrderValid = false;
    pendingPressedOrder.push(index);
    const navigatedBell = isBellNavi && pendingNaviOrderValid && index === expectedIndex;
    const naviMiss = isBellNavi && !pendingNaviOrderValid;

    if (reelTimers[index]) {
      clearInterval(reelTimers[index]);
      reelTimers[index] = null;
    }

    const finalPosition = chooseStopPosition(index, navigatedBell, naviMiss);

    // 通常当選の右リールは🟥7とBARが隣接しているため、
    // 最後だけ一瞬タメて「どっちに止まるか」をリールそのもので見せる。
    if (pendingEntryAmbiguous && index === 2) {
      stops[index].disabled = true;
      stops[index].classList.remove('active','nav-first');
      reels[index].classList.add('judging-stop');

      const pair = rightJudgePair();
      const nearPosition = pair
        ? (pendingRole === 'at' ? pair.bar : pair.seven)
        : findExactPosition(index, pendingRole === 'at' ? 'bar' : 'seven', 1);
      reelPositions[index] = nearPosition;
      reels[index].classList.remove('spinning','reverse-spinning');
      renderReel(index);
      settleReelArt(index);

      entryCinematicActive = true;
      autoTimers.push(setTimeout(() => {
        reelPositions[index] = finalPosition;
        reelStopped[index] = true;
        reels[index].classList.remove('spinning','reverse-spinning','judging-stop');
        renderReel(index);
        settleReelArt(index);
        entryCinematicActive = false;

        // ここで初めてAT/BONUSの答えを公開する。
        if (pendingRole === 'at') {
          showMachineCinematic('RESULT', 'AT 突入！', '🟥7 🟥7 🟥7', 'gold judge-result');
        } else {
          showMachineCinematic('RESULT', 'BONUS！', '🟥7 🟥7 BAR', 'red judge-result');
        }
        autoTimers.push(setTimeout(hideMachineCinematic, 650));
        if (reelStopped.every(Boolean)) finishGame();
      }, 280));
      return;
    }

    reelPositions[index] = finalPosition;
    reelStopped[index] = true;

    reels[index].classList.remove('spinning','reverse-spinning');
    renderReel(index);
    settleReelArt(index);
    stops[index].classList.remove('active','nav-first');
    stops[index].disabled = true;

    if (reelStopped.every(Boolean)) finishGame();
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
      const entryReplay = actualRole === 'hit'
        || actualRole === 'at'
        || actualRole === 'tier_up'
        || actualRole === 'freeze';
      els.eventTitle.textContent = eventLabels[e.type] || e.type.toUpperCase();
      els.eventNote.textContent = (e.note || '') + ' / 停止形: ' + roleLabels[actualRole]
        + (entryReplay ? ' / REPLAY' : (payout ? ' / ' + payout + '枚' : ''));
      els.eventBanner.className = 'event-banner' + (isPremium(e.type) ? ' premium' : isHot(e.type) ? ' hot' : '');
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
    if (!deferredEntryReveal || pendingSyntheticEntry) return raw;
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
    pendingSyntheticEntry = false;
    pendingSyntheticOmen = false;
    pendingEntryAmbiguous = false;
    clearBellNavi();

    // ここがレバーON抽選。直撃予約がある場合は、WASMを進めず
    // 「次Gの入賞表示」だけを1ゲーム挟む。
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
    pendingSyntheticOmen = false;
    if (atOmenFlow && atOmenFlow.phase === 'omen') {
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
    } else if (atOmenFlow && atOmenFlow.phase === 'entry') {
      // 予兆を経由した次Gで初めて当たり図柄を入賞させる。
      const queued = atOmenFlow;
      atOmenFlow = null;
      pendingSyntheticEntry = true;
      pendingEntryAmbiguous = false;
      pendingWasBonus = false;
      pendingWasAT = false;
      pendingWasChallenge = false;
      pendingResult = {
        events: queued.events,
        inAT: true,
        inBonus: true,
        episodeBonus: queued.episode,
        navOrder: -1,
        reelRole: 'replay',
        reelPayout: 3
      };
      pendingRole = 'hit';
    } else if (deferredEntryReveal) {
      const queued = deferredEntryReveal;
      deferredEntryReveal = null;
      pendingSyntheticEntry = true;
      pendingEntryAmbiguous = queued.ambiguous !== false && queued.role !== 'freeze';
      pendingWasBonus = false;
      pendingWasAT = false;
      pendingWasChallenge = false;
      pendingResult = {
        events: queued.events,
        inAT: queued.role === 'at' || queued.role === 'freeze',
        inBonus: queued.role === 'hit',
        navOrder: -1,
        reelRole: 'replay',
        reelPayout: 3
      };
      pendingRole = queued.role;
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

      // 旧WASMは中段チェリー/直撃で同GにAT/BONUS状態へ入ってしまう。
      // 現在Gは成立役だけで終え、開始イベントだけ次Gの🟥7/BONUS図柄入賞へ繰り越す。
      const normalTrigger = !pendingWasAT && !pendingWasBonus && !pendingWasChallenge;
      const physicalTriggerRole = pendingResult.reelRole || 'miss';
      const naturalTransition = normalTrigger
        && !forcedRole
        && (pendingResult.inBonus || pendingResult.inAT);
      if (naturalTransition) {
        const entryTypes = new Set([
          'bonus','episode_bonus','at_start','cold_enter','stock_gain','tier_up','freeze'
        ]);
        const entryEvents = (pendingResult.events || []).filter(e => entryTypes.has(e.type));
        const hasFreezeEntry = entryEvents.some(e => e.type === 'freeze');
        deferredEntryReveal = {
          role: hasFreezeEntry ? 'freeze' : (pendingResult.inBonus ? 'hit' : 'at'),
          events: entryEvents,
          // 通常時の当選は、FREEZE等の確定契機を除きAT/BONUSの行き先を演出で隠す。
          ambiguous: !hasFreezeEntry
        };

        // 現在Gは実際に成立した役だけを表示する。
        // AT/BONUS図柄は次Gの入賞ゲームまで出さない。
        pendingRole = physicalTriggerRole;
        pendingResult = {
          ...pendingResult,
          reelRole: physicalTriggerRole,
          events: (pendingResult.events || []).filter(e => !entryTypes.has(e.type)),
          inAT: false,
          inBonus: false
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

    pendingEntryReplayRole = pendingRole === 'hit'
      || pendingRole === 'at'
      || pendingRole === 'tier_up'
      || pendingRole === 'freeze';
    pendingPayout = pendingWasChallenge
      ? 0
      : (pendingWasBonus
          ? bonusVisibleReturnForRole(pendingRole)
          : (pendingEntryReplayRole
              ? 3
              : (forcedRole
                  ? accountingReturnForRole(forcedRole)
                  : Number(pendingResult.reelPayout || 0))));

    // 成立役・停止制御表・押し順をこの時点で固定する。
    pendingControl = buildSpinControl(pendingRole, pendingResult);
    pendingBellNaviActive = (pendingWasAT || pendingWasBonus)
      && pendingRole === 'bell9'
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
      pendingEntryAmbiguous ? 'entry_judge' : pendingRole,
      'spin'
    );

    if ((pendingSyntheticEntry || forcedRole === 'freeze') && pendingRole === 'freeze') {
      clearAutoTimers();
      runFreezeEntryCinematic();
      return;
    }

    if ((pendingSyntheticEntry || forcedRole === 'hit' || forcedRole === 'at') && (pendingRole === 'hit' || pendingRole === 'at')) {
      clearAutoTimers();

      // 通常時からの自然当選は最後のジャッジまでAT/BONUSを伏せる。
      // AT中予兆経由など、既に行き先を見せてよい経路だけ従来の確定演出。
      if (pendingEntryAmbiguous) {
        runNormalEntryJudgeCinematic();
      } else {
        runBonusEntryCinematic(pendingRole);
      }
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
      && pendingRole === 'bell9'
      && pendingControl
      && pendingControl.navOrder >= 0
      && !pendingNaviOrderValid;
    const payout = pendingPayout;
    const physicalLinePayoutRole = classifyPhysicalLinePayout();

    // ベル/リプレイは「内部予約役」より実停止ラインを優先する。
    // 5ラインのどこかで揃っていれば、その停止役が有効。
    const guaranteedPhysicalRole = guaranteedPayoutRoles.has(pendingRole)
      ? physicalLinePayoutRole
      : null;
    const visibleRole = naviMiss
      ? physicalPattern
      : (guaranteedPhysicalRole || physicalLinePayoutRole || pendingRole);
    const visiblePayout = guaranteedPhysicalRole
      ? accountingReturnForRole(guaranteedPhysicalRole)
      : (physicalLinePayoutRole
          ? accountingReturnForRole(physicalLinePayoutRole)
          : (naviMiss ? accountingReturnForRole(physicalPattern) : payout));
    let allEvents = [...(result.events || [])];

    // 通常時は、実停止5ラインで新たに成立したベル/リプレイも差枚へ反映。
    // AT/BONUSは内部純増会計済みなので二重加算しない。
    const accountingPayout = (!pendingWasAT && !pendingWasBonus && physicalLinePayoutRole)
      ? accountingReturnForRole(physicalLinePayoutRole)
      : payout;
    if (accountingPayout > 0 && !pendingWasBonus && !pendingWasAT && !pendingSyntheticEntry && !pendingSyntheticOmen) {
      const payoutResult = callJson('slot_apply_reel_payout_json', ['number'], [accountingPayout]);
      allEvents = allEvents.concat(payoutResult.events || []);
    }

    els.roleResult.textContent = naviMiss
      ? 'ナビ外し / ' + (roleLabels[physicalPattern] || physicalPattern)
      : (physicalLinePayoutRole
          ? (roleLabels[physicalLinePayoutRole] || physicalLinePayoutRole)
          : (pendingBellNaviActive
              ? '🔔 押し順ベル'
              : (roleLabels[pendingRole] || pendingRole)));
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
                      : (pendingEntryReplayRole ? 'REPLAY' : visiblePayout + '枚')))));

    pushEvents(allEvents);
    showFinalBanner(allEvents, visibleRole, visiblePayout);
    if (naviMiss) {
      els.eventNote.textContent += ' / ナビ外し（内部成立: 押し順ベル）';
    }
    if (assistSubstitute) {
      els.eventNote.textContent += ' / 代用停止';
    }
    if (aimAssistUsed) {
      els.eventNote.textContent += ' / 目押しアシスト';
    }
    if (!naviMiss && !physicalLinePayoutRole && physicalPattern !== pendingRole && !assistSubstitute && !aimAssistUsed && pendingRole !== 'one_medal') {
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
    pendingPayout = 0;
    pendingWasAT = false;
    pendingWasBonus = false;
    pendingWasChallenge = false;
    pendingBellNaviActive = false;
    pendingEntryReplayRole = false;
    pendingPressedOrder = [];
    pendingNaviOrderValid = true;
    pendingControl = null;
    pendingSyntheticEntry = false;
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