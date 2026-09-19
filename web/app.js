(async () => {
  const $ = (s) => document.querySelector(s);
  const els = {
    mode: $('#mode'), tier: $('#tier'), table: $('#table'), gameCount: $('#gameCount'),
    atLeft: $('#atLeft'), stocks: $('#stocks'),
    eventBanner: $('#eventBanner'), eventTitle: $('#eventTitle'), eventNote: $('#eventNote'),
    normalPattern: $('#normalPattern'), ceiling: $('#ceiling'), atPattern: $('#atPattern'),
    netRate: $('#netRate'), totalDiff: $('#totalDiff'), history: $('#history'),
    debug: $('#debugState'), statusLamp: $('#statusLamp'), statusText: $('#statusText'),
    lever: $('#lever'), autoToggle: $('#autoToggle'), reset: $('#reset'),
    roleResult: $('#roleResult'), payoutResult: $('#payoutResult'), roleTest: $('#roleTest'),
    stageScreen: $('#stageScreen'), characterSprite: $('#characterSprite'),
    effectLayer: $('#effectLayer'), cutinLayer: $('#cutinLayer'),
    cutinEyebrow: $('#cutinEyebrow'), cutinTitle: $('#cutinTitle'),
    cutinSub: $('#cutinSub'), stageCaption: $('#stageCaption')
  };

  const reels = [$('#reel1'), $('#reel2'), $('#reel3')];
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
    ['seven','replay','miss','cherry','bar','replay','bell','watermelon','miss','alt-seven','replay','penguin','cherry','bar','miss','replay','bell','chance','watermelon','miss','replay'],
    ['bell','replay','miss','seven','watermelon','replay','bar','miss','penguin','replay','alt-seven','miss','cherry','replay','bar','watermelon','miss','chance','bell','replay','miss'],
    ['replay','miss','bell','penguin','replay','bar','miss','replay','watermelon','chance','alt-seven','seven','bar','replay','miss','bell','replay','bar','miss','replay','miss']
  ];

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
    cz:'CZ', bonus:'BONUS', episode_bonus:'EPISODE BONUS', at_start:'AT START',
    at_add_games:'G数上乗せ', special_zone:'特化ZONE', upper_special_zone:'上位特化ZONE',
    stock_gain:'STOCK', tier_up:'昇格', tier_down:'転落', at_end:'AT END',
    upper_comeback:'上位引き戻し', freeze:'FREEZE', section_cross:'有利区間 CROSS'
  };

  let gameActive = false;
  let pendingResult = null;
  let pendingRole = 'miss';
  let pendingPayout = 0;
  let reelTimers = [null, null, null];
  let reelStopped = [true, true, true];
  let reelPositions = [0, 0, 0];
  let autoEnabled = false;
  let autoTimers = [];

  const mod = (n, m) => ((n % m) + m) % m;

  const clearStageClasses = () => {
    els.effectLayer.className = 'effect-layer';
    els.cutinLayer.className = 'cutin-layer';
    els.stageScreen.classList.remove('role-penguin','role-strong','role-hit','freeze-lock');
  };

  const stageCue = (role, phase = 'result') => {
    clearStageClasses();

    const cue = {
      one_medal: { fx:'', title:'1枚役', sub:'静かな払い出し', eyebrow:'NORMAL' },
      bell9: { fx:'yellow', title:'BELL', sub:'9枚', eyebrow:'YELLOW' },
      bell15: { fx:'gold', title:'BIG BELL', sub:'15枚', eyebrow:'GOLD' },
      replay: { fx:'blue', title:'REPLAY', sub:'もう一度', eyebrow:'BLUE' },
      weak_cherry: { fx:'green', title:'CHERRY', sub:'弱チェリー', eyebrow:'GREEN' },
      strong_cherry: { fx:'red slash', title:'強チェリー', sub:'中段チェリー', eyebrow:'RED', cls:'role-strong' },
      weak_chance: { fx:'purple', title:'CHANCE', sub:'弱チャンス目', eyebrow:'PURPLE' },
      strong_chance: { fx:'red slash', title:'強チャンス目', sub:'🐧 🍒 🐧', eyebrow:'RED', cls:'role-strong' },
      penguin_chance: { fx:'stripe', title:'PENGUIN CHANCE', sub:'🐧 🐧 🐧', eyebrow:'BLUE × LIGHT BLUE', cls:'role-penguin' },
      hit: { fx:'red flash', title:'HIT', sub:'🟥7 🟥7 BAR', eyebrow:'RED', cls:'role-hit' },
      at: { fx:'gold flash', title:'AT START', sub:'🟥7 🟥7 🟥7', eyebrow:'GOLD', cls:'role-hit' },
      tier_up: { fx:'gold', title:'AT 昇格', sub:'BAR BAR BAR', eyebrow:'GOLD', cls:'role-hit' },
      freeze: { fx:'stripe flash', title:'FREEZE', sub:'🟦7 🟦7 🟦7', eyebrow:'PREMIUM', cls:'freeze-lock' },
      watermelon: { fx:'green', title:'WATERMELON', sub:'スイカ', eyebrow:'GREEN' },
      miss: { fx:'', title:'', sub:'', eyebrow:'' }
    }[role] || { fx:'', title:'', sub:'', eyebrow:'' };

    if (cue.cls) els.stageScreen.classList.add(...cue.cls.split(' '));
    if (cue.fx) els.effectLayer.className = 'effect-layer ' + cue.fx;

    if (phase === 'spin') {
      if (['strong_cherry','strong_chance','penguin_chance','hit','at','tier_up','freeze'].includes(role)) {
        els.stageCaption.textContent = role === 'freeze' ? '……' : '気配がする…';
        if (role === 'freeze') els.effectLayer.className = 'effect-layer stripe';
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
    let scene = 'normal';
    let caption = '通常ステージ';
    if (s.normalMode === 'special') {
      scene = 'special';
      caption = '特殊ステージ';
    }
    if (s.inAT) {
      scene = s.atTier === 'upper' ? 'upper' : 'at';
      caption = s.atTier === 'upper' ? '上位AT' : 'AT';
    }
    els.stageScreen.dataset.scene = scene;
    els.stageCaption.textContent = caption;
  };

  const visibleKind = (index, position, row) => {
    const strip = reelStrips[index];
    const offset = row - 1; // 0=上段, 1=中段, 2=下段
    return strip[mod(position + offset, strip.length)];
  };

  const renderReel = (index) => {
    const position = reelPositions[index];
    const spans = [...reels[index].querySelectorAll(':scope > span')];
    spans.forEach((el, row) => {
      const symbol = symbolByKind[visibleKind(index, position, row)];
      el.dataset.kind = symbol.kind;
      el.innerHTML = symbol.html;
    });
  };

  const centerKind = (index, position = reelPositions[index]) => visibleKind(index, position, 1);

  const findExactPosition = (index, kind, row = 1) => {
    const strip = reelStrips[index];
    for (let pos = 0; pos < strip.length; pos++) {
      if (visibleKind(index, pos, row) === kind) return pos;
    }
    return reelPositions[index];
  };

  const classifyPattern = (positions = reelPositions) => {
    const center = [0,1,2].map(i => centerKind(i, positions[i]));
    const diagUp = [
      visibleKind(0, positions[0], 2),
      visibleKind(1, positions[1], 1),
      visibleKind(2, positions[2], 0)
    ];

    if (center.every(k => k === 'alt-seven')) return 'freeze';
    if (center.every(k => k === 'seven')) return 'at';
    if (center.every(k => k === 'bar')) return 'tier_up';
    if (center[0] === 'seven' && center[1] === 'seven' && center[2] === 'bar') return 'hit';

    if (center.every(k => k === 'penguin')) return 'penguin_chance';
    if (center[0] === 'penguin' && center[1] === 'cherry' && center[2] === 'penguin') return 'strong_chance';
    if (center.filter(k => k === 'penguin').length === 2) return 'weak_chance';

    const leftBottom = visibleKind(0, positions[0], 2);
    if (leftBottom === 'cherry' && center[1] === 'replay') return 'weak_cherry';
    if (center[0] === 'cherry' && center[1] !== 'replay') return 'strong_cherry';

    if (center.every(k => k === 'bell')) return 'bell9';
    if (diagUp.every(k => k === 'bell')) return 'bell15';
    if (center.every(k => k === 'watermelon')) return 'watermelon';
    if (center.every(k => k === 'replay')) return 'replay';

    return 'miss';
  };

  const accountingReturnForRole = (role) => {
    if (role === 'one_medal') return 1;
    if (role === 'bell9') return 9;
    if (role === 'bell15') return 15;
    if (role === 'replay') return 3; // 3枚BETを差枚会計上相殺
    return 0;
  };

  const deriveRoleFromResult = (result) => {
    const forced = els.roleTest.value;
    if (forced) return forced;

    const events = result.events || [];
    const types = new Set(events.map(e => e.type));
    if (types.has('freeze')) return 'freeze';
    if (types.has('bonus') || types.has('episode_bonus')) return 'hit';
    if (types.has('tier_up')) return 'tier_up';
    if (types.has('at_start')) return 'at';

    return result.reelRole || 'miss';
  };

  const targetForRole = (role, index) => {
    switch (role) {
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
        if (index === 1) return { row:1, notKind:'replay' };
        return null;
      case 'bell9': return { row:1, kind:'bell' };
      case 'bell15':
        return { row:index === 0 ? 2 : index === 1 ? 1 : 0, kind:'bell' };
      case 'watermelon': return { row:1, kind:'watermelon' };
      case 'replay': return { row:1, kind:'replay' };
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

  const chooseStopPosition = (index) => {
    const strip = reelStrips[index];
    const base = mod(reelPositions[index], strip.length);

    // 青7フリーズのみ0〜4コマ制御の外。押下位置を無視して中段へ強制揃い。
    if (pendingRole === 'freeze') {
      return findExactPosition(index, 'alt-seven', 1);
    }

    const target = targetForRole(pendingRole, index);
    if (target) {
      for (let slip = 0; slip <= 4; slip++) {
        const candidate = mod(base + slip, strip.length);
        if (candidateMatchesTarget(index, candidate, target)) return candidate;
      }
      // 引き込めない押し位置では取りこぼし。5コマ以上は滑らせない。
      return base;
    }

    // 1枚役は専用停止形をまだ持たないので、見た目はハズレ系停止形へ逃がす。
    const physicalRole = pendingRole === 'one_medal' ? 'miss' : pendingRole;

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
    reelStopped[index] = false;
    reels[index].classList.add('spinning');
    stops[index].disabled = false;
    stops[index].classList.add('active');

    reelTimers[index] = setInterval(() => {
      reelPositions[index] = mod(reelPositions[index] + 1, reelStrips[index].length);
      renderReel(index);
    }, 70);
  };

  const stopReelMotion = (index) => {
    if (!gameActive || reelStopped[index]) return;

    if (reelTimers[index]) {
      clearInterval(reelTimers[index]);
      reelTimers[index] = null;
    }

    reelPositions[index] = chooseStopPosition(index);
    renderReel(index);
    reelStopped[index] = true;

    reels[index].classList.remove('spinning');
    stops[index].classList.remove('active');
    stops[index].disabled = true;

    if (reelStopped.every(Boolean)) finishGame();
  };

  const isPremium = (type) => ['freeze','upper_special_zone'].includes(type);
  const isHot = (type) => ['episode_bonus','at_start','special_zone','upper_comeback','section_cross'].includes(type);

  const pushEvents = (events) => {
    for (const e of events) {
      history.unshift({ ...e, at: new Date().toLocaleTimeString('ja-JP',{hour12:false}) });
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
      els.eventTitle.textContent = eventLabels[e.type] || e.type.toUpperCase();
      els.eventNote.textContent = (e.note || '') + ' / 停止形: ' + roleLabels[actualRole] + (payout ? ' / ' + payout + '枚' : '');
      els.eventBanner.className = 'event-banner' + (isPremium(e.type) ? ' premium' : isHot(e.type) ? ' hot' : '');
    } else {
      els.eventTitle.textContent = roleLabels[actualRole] || 'NO HIT';
      els.eventNote.textContent = payout ? payout + '枚払出' : '次ゲームへ';
      els.eventBanner.className = 'event-banner';
    }
  };

  const render = (s) => {
    els.mode.textContent = labels[s.normalMode] || s.normalMode;
    els.tier.textContent = s.inAT ? (labels[s.atTier] || s.atTier) : '---';
    els.table.textContent = s.inAT ? (labels[s.atTable] || s.atTable) : '---';
    els.gameCount.textContent = s.totalGames.toLocaleString();
    els.atLeft.textContent = s.inAT ? s.atGamesLeft + 'G' : '0G';
    els.stocks.textContent = s.stocks;
    els.normalPattern.textContent = 'P' + s.normalPattern;
    els.ceiling.textContent = '天井 ' + s.normalCeiling + 'G / 表示 ' + s.normalDisplayGames + 'G / 実 ' + s.normalActualGames + 'G';
    els.atPattern.textContent = s.inAT ? 'P' + s.atPattern : '-';
    els.netRate.textContent = s.inAT ? '純増 約' + (s.atTier === 'upper' ? '12' : '6') + '枚/G' : '純増 -';
    els.totalDiff.textContent = (s.totalDiff >= 0 ? '+' : '') + s.totalDiff.toLocaleString();
    els.debug.textContent = JSON.stringify(s, null, 2);
    els.statusLamp.className = 'status-lamp ' + (s.inAT ? 'at' : 'live');
    els.statusText.textContent = s.inAT ? 'AT' : 'NORMAL';
    updateStageScene(s);
  };

  const beginGame = () => {
    if (gameActive) return;

    gameActive = true;
    pendingResult = null;
    reelStopped = [false, false, false];

    els.lever.disabled = true;
    els.roleResult.textContent = '回転中';
    els.payoutResult.textContent = '---';
    els.eventTitle.textContent = 'SPINNING';
    els.eventNote.textContent = 'STOPボタンでリールを止めろ';
    els.eventBanner.className = 'event-banner';

    const s0 = state();
    pendingResult = callJson(s0.inAT ? 'slot_spin_at_json' : 'slot_spin_normal_json');
    pendingRole = deriveRoleFromResult(pendingResult);

    const forcedRole = els.roleTest.value;
    pendingPayout = forcedRole
      ? accountingReturnForRole(forcedRole)
      : Number(pendingResult.reelPayout || 0);

    for (let i = 0; i < 3; i++) startReelMotion(i);

    if (autoEnabled) {
      clearAutoTimers();
      autoTimers.push(setTimeout(() => stopReelMotion(0), 450));
      autoTimers.push(setTimeout(() => stopReelMotion(1), 700));
      autoTimers.push(setTimeout(() => stopReelMotion(2), 950));
    }
  };

  const finishGame = () => {
    if (!gameActive) return;

    gameActive = false;
    els.lever.disabled = false;

    const result = pendingResult || { events: [] };
    pendingResult = null;

    const actualRole = classifyPattern();
    const payout = pendingPayout;
    let allEvents = [...(result.events || [])];

    if (payout > 0) {
      const payoutResult = callJson('slot_apply_reel_payout_json', ['number'], [payout]);
      allEvents = allEvents.concat(payoutResult.events || []);
    }

    // 成立役はC++抽選結果を表示。停止形そのものはactualRoleで内部確認可能。
    els.roleResult.textContent = roleLabels[pendingRole] || pendingRole;
    els.payoutResult.textContent = pendingRole === 'replay' ? 'REPLAY' : payout + '枚';

    pushEvents(allEvents);
    showFinalBanner(allEvents, pendingRole, pendingRole === 'replay' ? 0 : payout);
    render(state());

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
      if (!reelStopped[0]) autoTimers.push(setTimeout(() => stopReelMotion(0), 250));
      if (!reelStopped[1]) autoTimers.push(setTimeout(() => stopReelMotion(1), 500));
      if (!reelStopped[2]) autoTimers.push(setTimeout(() => stopReelMotion(2), 750));
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
      reels[i].classList.remove('spinning');
      stops[i].classList.remove('active');
      stops[i].disabled = true;
    });

    gameActive = false;
    pendingResult = null;
    pendingRole = 'miss';
    pendingPayout = 0;
    reelStopped = [true, true, true];
    els.lever.disabled = false;

    const seed = BigInt(Date.now());
    Module.ccall('slot_reset', null, ['number','number'], [
      Number(seed & 0xffffffffn),
      Number(seed >> 32n)
    ]);

    history.length = 0;
    els.history.innerHTML = '';
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