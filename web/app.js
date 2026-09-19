(async () => {
  const $ = (s) => document.querySelector(s);
  const els = {
    mode: $('#mode'), tier: $('#tier'), table: $('#table'), gameCount: $('#gameCount'),
    atLeft: $('#atLeft'), stocks: $('#stocks'),
    eventBanner: $('#eventBanner'), eventTitle: $('#eventTitle'),
    eventNote: $('#eventNote'), normalPattern: $('#normalPattern'), ceiling: $('#ceiling'),
    atPattern: $('#atPattern'), netRate: $('#netRate'), totalDiff: $('#totalDiff'),
    history: $('#history'), debug: $('#debugState'), statusLamp: $('#statusLamp'),
    statusText: $('#statusText'), lever: $('#lever'), autoToggle: $('#autoToggle'), reset: $('#reset')
  };

  const reels = [$('#reel1'), $('#reel2'), $('#reel3')];
  const stops = [...document.querySelectorAll('.stop')];
  const history = [];
  const symbols = [
    { kind:'seven', html:'7' },
    { kind:'alt-seven', html:'7<small>BLUE</small>' },
    { kind:'bell', html:'🔔<small>BELL</small>' },
    { kind:'cherry', html:'🍒<small>CHERRY</small>' },
    { kind:'replay', html:'↻<small>REPLAY</small>' },
    { kind:'bar', html:'BAR' },
    { kind:'chance', html:'★<small>CHANCE</small>' },
    { kind:'miss', html:'×<small>ハズレ</small>' }
  ];
  const symbolByKind = Object.fromEntries(symbols.map(s => [s.kind, s]));

  // 固定リール配列。回転中も停止時もこの配列からしか図柄は出さない。
  const reelStrips = [
    ['seven','bell','replay','miss','cherry','bar','bell','chance','miss','replay','alt-seven','bell','miss','cherry','replay','bar','bell','miss','chance','replay','bell'],
    ['bell','miss','replay','cherry','seven','bell','bar','miss','replay','chance','bell','alt-seven','miss','replay','cherry','bell','bar','miss','replay','chance','bell'],
    ['replay','bell','miss','bar','cherry','replay','bell','seven','miss','chance','replay','bell','alt-seven','miss','bar','replay','cherry','bell','miss','chance','replay']
  ];

  let Module;
  try {
    Module = await createSlotModule();
  } catch (err) {
    els.eventTitle.textContent = 'WASM LOAD ERROR';
    els.eventNote.textContent = 'GitHub Actionsで slot.js / slot.wasm をビルドしてください';
    els.debug.textContent = String(err);
    return;
  }

  const callJson = (name) => {
    const ptr = Module.ccall(name, 'number', [], []);
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
  let reelTimers = [null, null, null];
  let reelStopped = [true, true, true];
  let reelPositions = [0, 0, 0];
  let autoEnabled = false;
  let autoTimers = [];

  const mod = (n, m) => ((n % m) + m) % m;

  const renderReel = (index) => {
    const strip = reelStrips[index];
    const center = mod(reelPositions[index], strip.length);
    const kinds = [
      strip[mod(center - 1, strip.length)],
      strip[center],
      strip[mod(center + 1, strip.length)]
    ];

    const spans = [...reels[index].querySelectorAll(':scope > span')];
    spans.forEach((el, row) => {
      const symbol = symbolByKind[kinds[row]];
      el.dataset.kind = symbol.kind;
      el.innerHTML = symbol.html;
    });
  };

  const centerKind = (index, position = reelPositions[index]) => {
    const strip = reelStrips[index];
    return strip[mod(position, strip.length)];
  };

  const isSevenKind = (kind) => kind === 'seven' || kind === 'alt-seven';

  // まだ「どの成立役で赤7/青7を揃えるか」は未決定。
  // そのため現段階では、停止制御で accidental 7揃いを必ず回避する。
  const chooseStopPosition = (index) => {
    const strip = reelStrips[index];
    const base = mod(reelPositions[index], strip.length);

    for (let slip = 0; slip <= 4; slip++) {
      const candidate = mod(base + slip, strip.length);

      // 今止めるリールが最後なら、中央ラインの3リールが全て7系になる候補を禁止。
      const willAllStop = reelStopped.filter(Boolean).length === 2;
      if (willAllStop) {
        const kinds = [0,1,2].map(i => i === index ? centerKind(i, candidate) : centerKind(i));
        if (kinds.every(isSevenKind)) continue;
      }

      return candidate;
    }

    // 配列上0〜4コマ全てが不適切なケースは通常起きないが、保険で5コマ目には進めず現在位置。
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

    // STOP入力位置から0〜4コマの範囲で停止位置を決定。
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

    if (events.length) {
      const e = events[events.length - 1];
      els.eventTitle.textContent = eventLabels[e.type] || e.type.toUpperCase();
      els.eventNote.textContent = e.note || '';
      els.eventBanner.className = 'event-banner' + (isPremium(e.type) ? ' premium' : isHot(e.type) ? ' hot' : '');
    } else {
      els.eventTitle.textContent = 'NO HIT';
      els.eventNote.textContent = '次ゲームへ';
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
    els.ceiling.textContent = '天井 ' + s.normalCeiling + 'G / 現在 ' + s.normalGames + 'G';
    els.atPattern.textContent = s.inAT ? 'P' + s.atPattern : '-';
    els.netRate.textContent = s.inAT ? '純増 約' + (s.atTier === 'upper' ? '12' : '6') + '枚/G' : '純増 -';
    els.totalDiff.textContent = (s.totalDiff >= 0 ? '+' : '') + s.totalDiff.toLocaleString();
    els.debug.textContent = JSON.stringify(s, null, 2);
    els.statusLamp.className = 'status-lamp ' + (s.inAT ? 'at' : 'live');
    els.statusText.textContent = s.inAT ? 'AT' : 'NORMAL';
  };

  const beginGame = () => {
    if (gameActive) return;

    gameActive = true;
    pendingResult = null;
    reelStopped = [false, false, false];

    els.lever.disabled = true;
    els.eventTitle.textContent = 'SPINNING';
    els.eventNote.textContent = 'STOPボタンでリールを止めろ';
    els.eventBanner.className = 'event-banner';

    const s0 = state();
    pendingResult = callJson(s0.inAT ? 'slot_spin_at_json' : 'slot_spin_normal_json');

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

    pushEvents(result.events || []);
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
    reelStopped = [true, true, true];
    els.lever.disabled = false;

    const seed = BigInt(Date.now());
    Module.ccall('slot_reset', null, ['number','number'], [
      Number(seed & 0xffffffffn),
      Number(seed >> 32n)
    ]);

    history.length = 0;
    els.history.innerHTML = '';
    els.eventTitle.textContent = 'RESET';
    els.eventNote.textContent = 'レバーを叩け';
    els.eventBanner.className = 'event-banner';
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