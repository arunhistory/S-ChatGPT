(async () => {
  const $ = (s) => document.querySelector(s);
  const els = {
    mode: $('#mode'), tier: $('#tier'), table: $('#table'), gameCount: $('#gameCount'),
    atLeft: $('#atLeft'), stocks: $('#stocks'),
    eventBanner: $('#eventBanner'), eventTitle: $('#eventTitle'),
    eventNote: $('#eventNote'), normalPattern: $('#normalPattern'), ceiling: $('#ceiling'),
    atPattern: $('#atPattern'), netRate: $('#netRate'), totalDiff: $('#totalDiff'),
    history: $('#history'), debug: $('#debugState'), statusLamp: $('#statusLamp'),
    statusText: $('#statusText')
  };
  const reels = [$('#reel1'), $('#reel2'), $('#reel3')];
  const stops = [...document.querySelectorAll('.stop')];
  const history = [];
  const symbols = ['7','BAR','◆','●','★','CHANCE','BONUS'];

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

  const randomizeReel = (el) => {
    const spans = [...el.querySelectorAll('span')];
    spans.forEach(s => s.textContent = symbols[Math.floor(Math.random()*symbols.length)]);
  };
  const animateReels = () => {
    reels.forEach((r,i) => {
      r.classList.add('spinning');
      setTimeout(() => { randomizeReel(r); r.classList.remove('spinning'); }, 90 + i*80);
    });
    stops.forEach((b,i) => {
      b.classList.add('active');
      setTimeout(() => b.classList.remove('active'), 160 + i*80);
    });
  };

  const isPremium = (type) => ['freeze','upper_special_zone'].includes(type);
  const isHot = (type) => ['episode_bonus','at_start','special_zone','upper_comeback','section_cross'].includes(type);

  const pushEvents = (events) => {
    for (const e of events) {
      history.unshift({ ...e, at: new Date().toLocaleTimeString('ja-JP',{hour12:false}) });
    }
    history.splice(14);
    els.history.innerHTML = history.map(e =>
      '<li class="'+(isPremium(e.type)?'premium':'')+'"><b>'+ (eventLabels[e.type] || e.type) +'</b><span>'+ (e.note || '') +(e.value ? ' ['+e.value+']':'')+'</span></li>'
    ).join('');
    if (events.length) {
      const e = events[events.length - 1];
      els.eventTitle.textContent = eventLabels[e.type] || e.type.toUpperCase();
      els.eventNote.textContent = e.note || '';
      els.eventBanner.className = 'event-banner' + (isPremium(e.type)?' premium':isHot(e.type)?' hot':'');
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
    els.debug.textContent = JSON.stringify(s,null,2);
    els.statusLamp.className = 'status-lamp ' + (s.inAT ? 'at' : 'live');
    els.statusText.textContent = s.inAT ? 'AT' : 'NORMAL';
  };

  const spin = () => {
    const s0 = state();
    animateReels();
    const result = callJson(s0.inAT ? 'slot_spin_at_json' : 'slot_spin_normal_json');
    pushEvents(result.events || []);
    render(state());
  };

  $('#lever').onclick = spin;
  stops.forEach(b => b.onclick = () => b.classList.toggle('active'));
  $('#auto10').onclick = () => {
    let i = 0;
    const run = () => {
      if (i++ >= 10) return;
      spin();
      setTimeout(run, 110);
    };
    run();
  };
  $('#reset').onclick = () => {
    const seed = BigInt(Date.now());
    Module.ccall('slot_reset', null, ['number','number'], [Number(seed & 0xffffffffn), Number(seed >> 32n)]);
    history.length = 0;
    els.history.innerHTML = '';
    els.eventTitle.textContent = 'RESET';
    els.eventNote.textContent = '新しいシードで開始';
    render(state());
  };

  render(state());
})();