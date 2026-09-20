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
    naviState: $('#naviState'),
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
    cold_enter:'冷遇', section_reward:'有利区間ルーレット'
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
  let pendingSyntheticEntry = false;
  let reelTimers = [null, null, null];
  let reelStopped = [true, true, true];
  let reelPositions = [0, 0, 0];
  let autoEnabled = false;
  let autoTimers = [];

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
    els.stageScreen.classList.remove('role-penguin','role-strong','role-hit','freeze-lock');
  };

  const stageCue = (role, phase = 'result') => {
    clearStageClasses();

    const cue = {
      one_medal: { fx:'', title:'1枚役', sub:'静かな払い出し', eyebrow:'NORMAL' },
      bell9: { fx:'yellow', title:'BELL', sub:'9枚', eyebrow:'YELLOW' },
      bell15: { fx:'gold', title:'BIG BELL', sub:'15枚', eyebrow:'GOLD' },
      replay: { fx:'blue', title:'REPLAY', sub:'もう一度', eyebrow:'BLUE' },
      three_medal: { fx:'', title:'3枚役', sub:'3枚', eyebrow:'NORMAL' },
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
    if (s.inBonus) {
      scene = 'at';
      caption = s.episodeBonus ? 'EPISODE BONUS' : 'BONUS';
    } else if (s.challengeActive) {
      scene = 'normal';
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
    const safeKinds = ['chance','replay','bar','miss'];
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

    // strong_cherry は現行ゲーム仕様上「中段チェリー」。
    // レバーONで成立確定しているため、汎用取りこぼし/代用制御を通さない。
    if (pendingControl.role === 'strong_cherry') {
      return chooseMiddleCherryPosition(index, base);
    }

    const target = pendingControl.targets[index];
    if (target) {
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

    reelPositions[index] = chooseStopPosition(index, navigatedBell, naviMiss);
    renderReel(index);
    reelStopped[index] = true;

    reels[index].classList.remove('spinning');
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
    clearBellNavi();

    // ここがレバーON抽選。直撃予約がある場合は、WASMを進めず
    // 「次Gの入賞表示」だけを1ゲーム挟む。
    const forcedRole = els.roleTest.value;
    if (forcedRole) els.roleTest.value = '';

    pendingSyntheticEntry = false;
    if (deferredEntryReveal) {
      const queued = deferredEntryReveal;
      deferredEntryReveal = null;
      pendingSyntheticEntry = true;
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
          events: entryEvents
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

    stageCue(pendingRole, 'spin');
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
    const visibleRole = naviMiss
      ? physicalPattern
      : (physicalLinePayoutRole || pendingRole);
    const visiblePayout = physicalLinePayoutRole
      ? accountingReturnForRole(physicalLinePayoutRole)
      : (naviMiss ? accountingReturnForRole(physicalPattern) : payout);
    let allEvents = [...(result.events || [])];

    // 通常時は、実停止5ラインで新たに成立したベル/リプレイも差枚へ反映。
    // AT/BONUSは内部純増会計済みなので二重加算しない。
    const accountingPayout = (!pendingWasAT && !pendingWasBonus && physicalLinePayoutRole)
      ? accountingReturnForRole(physicalLinePayoutRole)
      : payout;
    if (accountingPayout > 0 && !pendingWasBonus && !pendingWasAT && !pendingSyntheticEntry) {
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
      reels[i].classList.remove('spinning');
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
    deferredEntryReveal = null;
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