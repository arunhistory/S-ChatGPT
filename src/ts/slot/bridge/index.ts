import {
  AcquisitionResult,
  AcquisitionStatus,
  ATTier,
  ATWindowStatus,
  NormalMode,
  ATResolvedEvent,
  ATResolutionStatus,
  ATEventBit,
  CommandStatus,
  BonusCycleOutcome,
  BonusKind,
  BonusTransitionOutcome,
  CZFinalizeOutcome,
  EntryTarget,
  EntryGateKind,
  LeverResult,
  LowerFallPhase,
  LowerFallPushOutcome,
  MachineArea,
  PendingEvent,
  ReelId,
  ReelPosition,
  RoleFlag,
  RevivalOutcome,
  SessionPhase,
  SectionRewardKind,
  SpecialHit,
  SpecialResult,
  StopResult,
  StopStatus,
  SpecialZoneHitResult,
  StockProfile,
  Symbol,
} from "../types.js";

export interface SlotWasmV2 {
  slot_v2_reset(seedLo: number, seedHi: number): void;
  slot_v2_lever(): number;
  slot_v2_stop(reel: number, pressedPosition: number): number;
  slot_v2_phase(): number;
  slot_v2_special_result(): number;
  slot_v2_complete_special(): number;
  slot_v2_stop_sequence(orderIndex: number): number;
  slot_v2_stopped_position(reel: number): number;
  slot_v2_acquisition(): number;
  slot_v2_symbol_at(reel: number, position: number): number;
  slot_v2_visible_symbol(reel: number, centerPosition: number, rowOffset: number): number;
  slot_v2_validate_left(): number;
  slot_v2_preflight(): number;
  slot_v2_validate_reel(reel: number): number;
  slot_v2_assist_failure_mask(reel: number, role: number): number;
  slot_v2_reel_ready_mask(): number;
  slot_v2_last_special(): number;
  slot_v2_last_role(): number;
  slot_v2_freeze_active(): number;
  slot_v2_machine_area(): number;
  slot_v2_at_active(): number;
  slot_v2_at_tier(): number;
  slot_v2_at_games_left(): number;
  slot_v2_special_committed(): number;
  slot_v2_section_diff(): bigint;
  slot_v2_section_minimum(): bigint;
  slot_v2_section_count(): bigint;
  slot_v2_stock_count(): number;
  slot_v2_point_count(): bigint;
  slot_v2_pending_events(): number;
  slot_v2_section_reward(): number;
  slot_v2_section_transition(): number;
  slot_v2_special_zone(): number;
  slot_v2_special_zone_result(): number;
  slot_v2_bonus_state(): number;
  slot_v2_bonus_cycle(): number;
  slot_v2_bonus_transition(): number;
  slot_v2_upper_comeback(): number;
  slot_v2_at_single_transition(): number;
  slot_v2_normal_at_trigger(): number;
  slot_v2_revival_state(): number;
  slot_v2_revival_game(): number;
  slot_v2_revival_finalize(): number;
  slot_v2_entry_gate(): number;
  slot_v2_at_window(): number;
  slot_v2_lower_fall_push(): number;
  slot_v2_lower_fall_challenge(): number;
  slot_v2_lower_fall_push_outcome(): number;
  slot_v2_at_cycle(): number;
  slot_v2_at_resolution(): number;
  slot_v2_normal_mode(): number;
  slot_v2_normal_route(): number;
  slot_v2_normal_ceiling_transition(): number;
  slot_v2_normal_actual_games(): number;
  slot_v2_normal_display_games(): number;
  slot_v2_normal_progress(): number;
  slot_v2_cz_cycle(): number;
  slot_v2_cz_finalize(): number;
  slot_v2_ceiling_count(): number;
  slot_v2_ceiling_at(index: number): number;
  slot_v2_bell_navigation(): number;
  slot_v2_bell_navigation_next(): number;
  slot_v2_bell_navigation_correct(): number;
}

export function decodeLeverResult(packed: number): LeverResult {
  return {
    role: (packed & 0xff) as RoleFlag,
    special: ((packed >>> 8) & 0xff) as SpecialHit,
    mainLotteryRan: ((packed >>> 16) & 1) === 1,
    commandStatus: ((packed >>> 24) & 0xff) as CommandStatus,
  };
}

export function decodeStopResult(packed: number): StopResult {
  return {
    finalPosition: (packed & 0xff) as ReelPosition,
    slip: (packed >>> 8) & 0xff,
    status: ((packed >>> 16) & 0xff) as StopStatus,
  };
}

// TSは結果を決めない。C++/WASMが返した確定値を型付きで受け取るだけ。
export function lever(wasm: SlotWasmV2): LeverResult {
  return decodeLeverResult(wasm.slot_v2_lever() >>> 0);
}

export function stop(
  wasm: SlotWasmV2,
  reel: ReelId,
  pressedPosition: ReelPosition,
): StopResult {
  return decodeStopResult(
    wasm.slot_v2_stop(reel, pressedPosition) >>> 0,
  );
}


export interface LeftReelValidation {
  cherryHidePossible: boolean;
  bellGuaranteed: boolean;
  replayGuaranteed: boolean;
  barLandmarkPair: boolean;
}

export function validateLeftReel(wasm: SlotWasmV2): LeftReelValidation {
  const bits = wasm.slot_v2_validate_left() >>> 0;
  return {
    cherryHidePossible: (bits & 1) !== 0,
    bellGuaranteed: (bits & 2) !== 0,
    replayGuaranteed: (bits & 4) !== 0,
    barLandmarkPair: (bits & 8) !== 0,
  };
}


export function readAcquisition(wasm: SlotWasmV2): AcquisitionResult {
  const packed = wasm.slot_v2_acquisition() >>> 0;
  return {
    status: (packed & 0xff) as AcquisitionStatus,
    internalRole: ((packed >>> 8) & 0xff) as RoleFlag,
    medals: (packed >>> 16) & 0xffff,
  };
}


export function readSymbolAt(
  wasm: SlotWasmV2,
  reel: ReelId,
  position: ReelPosition,
): Symbol {
  return wasm.slot_v2_symbol_at(reel, position) as Symbol;
}

export function readVisibleSymbol(
  wasm: SlotWasmV2,
  reel: ReelId,
  centerPosition: ReelPosition,
  rowOffset: -1 | 0 | 1,
): Symbol {
  return wasm.slot_v2_visible_symbol(
    reel,
    centerPosition,
    rowOffset,
  ) as Symbol;
}


export function readSessionPhase(wasm: SlotWasmV2): SessionPhase {
  return wasm.slot_v2_phase() as SessionPhase;
}

export function readSpecialResult(wasm: SlotWasmV2): SpecialResult {
  const packed = wasm.slot_v2_special_result() >>> 0;
  return {
    hit: (packed & 0xff) as SpecialHit,
    target: ((packed >>> 8) & 0xff) as EntryTarget,
    stockProfile: ((packed >>> 16) & 0xff) as StockProfile,
    freeze: ((packed >>> 24) & 1) === 1,
  };
}

export function completeSpecial(wasm: SlotWasmV2): SessionPhase {
  return wasm.slot_v2_complete_special() as SessionPhase;
}


export function readStopSequence(
  wasm: SlotWasmV2,
  orderIndex: 0 | 1 | 2,
): ReelId | null {
  const value = wasm.slot_v2_stop_sequence(orderIndex) >>> 0;
  if (value === 0xffffffff) return null;
  return value as ReelId;
}


export interface AssistReelValidation {
  defined: boolean;
  bellGuaranteed: boolean;
  replayGuaranteed: boolean;
}

export function validateAssistReel(
  wasm: SlotWasmV2,
  reel: ReelId,
): AssistReelValidation {
  const bits = wasm.slot_v2_validate_reel(reel) >>> 0;
  return {
    defined: (bits & 1) !== 0,
    bellGuaranteed: (bits & 2) !== 0,
    replayGuaranteed: (bits & 4) !== 0,
  };
}

export function readReelReadyMask(wasm: SlotWasmV2): number {
  return wasm.slot_v2_reel_ready_mask() >>> 0;
}


export interface MachineSnapshot {
  area: MachineArea;
  sectionDiff: bigint;
  sectionMinimum: bigint;
  sectionCount: bigint;
  stockCount: number;
  pointCount: bigint;
}

export function readMachineSnapshot(wasm: SlotWasmV2): MachineSnapshot {
  return {
    area: wasm.slot_v2_machine_area() as MachineArea,
    sectionDiff: wasm.slot_v2_section_diff(),
    sectionMinimum: wasm.slot_v2_section_minimum(),
    sectionCount: wasm.slot_v2_section_count(),
    stockCount: wasm.slot_v2_stock_count() >>> 0,
    pointCount: wasm.slot_v2_point_count(),
  };
}


export interface ATSnapshot {
  active: boolean;
  tier: ATTier;
  gamesLeft: number;
  specialCommitted: boolean;
}

export function readATSnapshot(wasm: SlotWasmV2): ATSnapshot {
  return {
    active: wasm.slot_v2_at_active() !== 0,
    tier: wasm.slot_v2_at_tier() as ATTier,
    gamesLeft: wasm.slot_v2_at_games_left() | 0,
    specialCommitted: wasm.slot_v2_special_committed() !== 0,
  };
}


export interface BellNavigationSnapshot {
  active: boolean;
  orderIndex: number | null;
  requiredStops: number;
  nextReel: ReelId | null;
  correctSoFar: boolean;
}

export function readBellNavigation(
  wasm: SlotWasmV2,
): BellNavigationSnapshot {
  const packed = wasm.slot_v2_bell_navigation() >>> 0;
  const active = (packed & 1) !== 0;

  const nextRaw = wasm.slot_v2_bell_navigation_next() >>> 0;

  return {
    active,
    orderIndex: active ? ((packed >>> 8) & 0xff) : null,
    requiredStops: active ? ((packed >>> 16) & 0xff) : 0,
    nextReel: active && nextRaw !== 0xffffffff ? (nextRaw as ReelId) : null,
    correctSoFar: active && wasm.slot_v2_bell_navigation_correct() !== 0,
  };
}


export interface PendingEventSnapshot {
  rawBits: number;
  bellFiveAT: boolean;
  czThreeMissHit: boolean;
  nextHitAT: boolean;
  atHit: boolean;
  atFall: boolean;
  atAddGames: boolean;
  atSpecial: boolean;
  atEpisode: boolean;
  atUpperSpecial: boolean;
  atMultiple: boolean;
  sectionTierUp: boolean;
  sectionSpecial: boolean;
  sectionUpperSpecial: boolean;
  czHit: boolean;
  atWindowEmpty: boolean;
  atStockAvailable: boolean;
  bonusComplete: boolean;
  bonusEpisodeUpgrade: boolean;
  upperComebackHit: boolean;
  specialZoneAddGames: boolean;
  specialZoneBonus: boolean;
}

export function readPendingEvents(
  wasm: SlotWasmV2,
): PendingEventSnapshot {
  const bits = wasm.slot_v2_pending_events() >>> 0;

  return {
    rawBits: bits,
    bellFiveAT: (bits & PendingEvent.BellFiveAT) !== 0,
    czThreeMissHit: (bits & PendingEvent.CZThreeMissHit) !== 0,
    nextHitAT: (bits & PendingEvent.NextHitAT) !== 0,
    atHit: (bits & PendingEvent.ATHit) !== 0,
    atFall: (bits & PendingEvent.ATFall) !== 0,
    atAddGames: (bits & PendingEvent.ATAddGames) !== 0,
    atSpecial: (bits & PendingEvent.ATSpecial) !== 0,
    atEpisode: (bits & PendingEvent.ATEpisode) !== 0,
    atUpperSpecial: (bits & PendingEvent.ATUpperSpecial) !== 0,
    atMultiple: (bits & PendingEvent.ATMultiple) !== 0,
    sectionTierUp: (bits & PendingEvent.SectionTierUp) !== 0,
    sectionSpecial: (bits & PendingEvent.SectionSpecial) !== 0,
    sectionUpperSpecial: (bits & PendingEvent.SectionUpperSpecial) !== 0,
    czHit: (bits & PendingEvent.CZHit) !== 0,
    atWindowEmpty: (bits & PendingEvent.ATWindowEmpty) !== 0,
    atStockAvailable: (bits & PendingEvent.ATStockAvailable) !== 0,
    bonusComplete: (bits & PendingEvent.BonusComplete) !== 0,
    bonusEpisodeUpgrade: (bits & PendingEvent.BonusEpisodeUpgrade) !== 0,
    upperComebackHit: (bits & PendingEvent.UpperComebackHit) !== 0,
    specialZoneAddGames: (bits & PendingEvent.SpecialZoneAddGames) !== 0,
    specialZoneBonus: (bits & PendingEvent.SpecialZoneBonus) !== 0,
  };
}


export interface ATCycleSnapshot {
  active: boolean;
  hitStockGained: boolean;
  windowEmptyAfterGame: boolean;
  rawBits: number;
  hit: boolean;
  fall: boolean;
  addGames: boolean;
  special: boolean;
  episode: boolean;
  upperSpecial: boolean;
}

export interface ATResolutionSnapshot {
  status: ATResolutionStatus;
  event: ATResolvedEvent;
  count: number;
}

export function readATCycle(wasm: SlotWasmV2): ATCycleSnapshot {
  const packed = wasm.slot_v2_at_cycle() >>> 0;
  const bits = (packed >>> 8) & 0x3f;

  return {
    active: (packed & 1) !== 0,
    hitStockGained: (packed & (1 << 1)) !== 0,
    windowEmptyAfterGame: (packed & (1 << 2)) !== 0,
    rawBits: bits,
    hit: (bits & ATEventBit.Hit) !== 0,
    fall: (bits & ATEventBit.Fall) !== 0,
    addGames: (bits & ATEventBit.AddGames) !== 0,
    special: (bits & ATEventBit.Special) !== 0,
    episode: (bits & ATEventBit.Episode) !== 0,
    upperSpecial: (bits & ATEventBit.UpperSpecial) !== 0,
  };
}

export function readATResolution(
  wasm: SlotWasmV2,
): ATResolutionSnapshot {
  const packed = wasm.slot_v2_at_resolution() >>> 0;

  return {
    status: (packed & 0xff) as ATResolutionStatus,
    event: ((packed >>> 8) & 0xff) as ATResolvedEvent,
    count: (packed >>> 16) & 0xff,
  };
}

export function readNormalMode(wasm: SlotWasmV2): NormalMode {
  return wasm.slot_v2_normal_mode() as NormalMode;
}

export interface NormalRouteSnapshot {
  pattern: number;
  ceiling: number;
  specialWindowChecked: boolean;
  ceilingConsumed: boolean;
  freezeQueued: boolean;
}

export function readNormalRoute(
  wasm: SlotWasmV2,
): NormalRouteSnapshot {
  const packed = wasm.slot_v2_normal_route() >>> 0;
  return {
    pattern: packed & 0xff,
    ceiling: (packed >>> 8) & 0xffff,
    specialWindowChecked: (packed & (1 << 24)) !== 0,
    ceilingConsumed: (packed & (1 << 25)) !== 0,
    freezeQueued: (packed & (1 << 26)) !== 0,
  };
}

export function readNormalCeilingTransition(
  wasm: SlotWasmV2,
): number {
  return wasm.slot_v2_normal_ceiling_transition() >>> 0;
}


export interface NormalProgressSnapshot {
  actualGames: number;
  displayGames: number;
  czMisses: number;
  normalHitsWithoutAT: number;
  bell9Streak: number;
  nextHitATGuaranteed: boolean;
}

export interface CZCycleSnapshot {
  active: boolean;
  baseHit: boolean;
  gamesLeft: number;
  ended: boolean;
}

export function readNormalProgress(
  wasm: SlotWasmV2,
): NormalProgressSnapshot {
  const packed = wasm.slot_v2_normal_progress() >>> 0;

  return {
    actualGames: wasm.slot_v2_normal_actual_games() >>> 0,
    displayGames: wasm.slot_v2_normal_display_games() >>> 0,
    czMisses: packed & 0xff,
    normalHitsWithoutAT: (packed >>> 8) & 0xff,
    bell9Streak: (packed >>> 16) & 0xff,
    nextHitATGuaranteed: ((packed >>> 24) & 1) !== 0,
  };
}

export function readCZCycle(wasm: SlotWasmV2): CZCycleSnapshot {
  const packed = wasm.slot_v2_cz_cycle() >>> 0;

  return {
    active: (packed & 1) !== 0,
    baseHit: (packed & (1 << 1)) !== 0,
    gamesLeft: (packed >>> 8) & 0xff,
    ended: ((packed >>> 16) & 1) !== 0,
  };
}

export function readCeilingCatalog(wasm: SlotWasmV2): number[] {
  const count = wasm.slot_v2_ceiling_count() >>> 0;
  const values: number[] = [];

  for (let i = 0; i < count; ++i) {
    const value = wasm.slot_v2_ceiling_at(i) >>> 0;
    if (value !== 0xffffffff) values.push(value);
  }

  return values;
}


export interface SectionRewardSnapshot {
  cut: boolean;
  preferenceLevel: number;
  kind: SectionRewardKind;
}

export function readSectionReward(
  wasm: SlotWasmV2,
): SectionRewardSnapshot {
  const packed = wasm.slot_v2_section_reward() >>> 0;

  return {
    cut: (packed & 1) !== 0,
    preferenceLevel: (packed >>> 8) & 0xff,
    kind: ((packed >>> 16) & 0xff) as SectionRewardKind,
  };
}


export interface CZFinalizeSnapshot {
  outcome: CZFinalizeOutcome;
  leverBlocked: boolean;
}

export function readCZFinalize(
  wasm: SlotWasmV2,
): CZFinalizeSnapshot {
  const packed = wasm.slot_v2_cz_finalize() >>> 0;

  return {
    outcome: (packed & 0xff) as CZFinalizeOutcome,
    leverBlocked: ((packed >>> 8) & 1) !== 0,
  };
}


export interface SectionTransitionSnapshot {
  applied: boolean;
  tierChanged: boolean;
  specialStarted: boolean;
  upperSpecialPending: boolean;
  before: ATTier;
  after: ATTier;
}

export interface ATWindowSnapshot {
  status: ATWindowStatus;
  stockCount: number;
}

export function readSectionTransition(
  wasm: SlotWasmV2,
): SectionTransitionSnapshot {
  const packed = wasm.slot_v2_section_transition() >>> 0;

  return {
    applied: (packed & 1) !== 0,
    tierChanged: (packed & (1 << 1)) !== 0,
    specialStarted: (packed & (1 << 2)) !== 0,
    upperSpecialPending: (packed & (1 << 3)) !== 0,
    before: ((packed >>> 8) & 0xff) as ATTier,
    after: ((packed >>> 16) & 0xff) as ATTier,
  };
}

export function readATWindow(wasm: SlotWasmV2): ATWindowSnapshot {
  const packed = wasm.slot_v2_at_window() >>> 0;

  return {
    status: (packed & 0xff) as ATWindowStatus,
    stockCount: packed >>> 8,
  };
}

export interface LowerFallChallengeSnapshot {
  phase: LowerFallPhase;
  waitGameActive: boolean;
  judgeBell: boolean;
  savedGames: number;
  buttonReady: boolean;
  lastPushOutcome: LowerFallPushOutcome;
}

export function readLowerFallChallenge(
  wasm: SlotWasmV2,
): LowerFallChallengeSnapshot {
  const packed = wasm.slot_v2_lower_fall_challenge() >>> 0;
  const phase = (packed & 0xff) as LowerFallPhase;

  return {
    phase,
    waitGameActive: (packed & (1 << 8)) !== 0,
    judgeBell: (packed & (1 << 9)) !== 0,
    savedGames: (packed >>> 16) & 0xffff,
    buttonReady: phase === LowerFallPhase.ButtonReady,
    lastPushOutcome:
      wasm.slot_v2_lower_fall_push_outcome() as LowerFallPushOutcome,
  };
}

export function pushLowerFallChallenge(
  wasm: SlotWasmV2,
): LowerFallPushOutcome {
  return wasm.slot_v2_lower_fall_push() as LowerFallPushOutcome;
}


export interface SpecialZoneSnapshot {
  active: boolean;
  gamesLeft: number;
}

export function readSpecialZone(
  wasm: SlotWasmV2,
): SpecialZoneSnapshot {
  const packed = wasm.slot_v2_special_zone() >>> 0;

  return {
    active: (packed & 1) !== 0,
    gamesLeft: (packed >>> 8) & 0xff,
  };
}


export function readAssistFailurePositions(
  wasm: SlotWasmV2,
  reel: ReelId,
  role: RoleFlag,
): number[] {
  const mask = wasm.slot_v2_assist_failure_mask(reel, role) >>> 0;
  const positions: number[] = [];

  for (let i = 0; i < 21; ++i) {
    if ((mask & (1 << i)) !== 0) positions.push(i);
  }

  return positions;
}


export interface BonusSnapshot {
  active: boolean;
  kind: BonusKind;
  medalsLeft: number;
}

export interface BonusCycleSnapshot {
  activeBefore: boolean;
  kind: BonusKind;
  outcome: BonusCycleOutcome;
}

export function readBonusState(wasm: SlotWasmV2): BonusSnapshot {
  const packed = wasm.slot_v2_bonus_state() >>> 0;

  return {
    active: (packed & 1) !== 0,
    kind: ((packed >>> 8) & 0xff) as BonusKind,
    medalsLeft: (packed >>> 16) & 0xffff,
  };
}

export function readBonusCycle(wasm: SlotWasmV2): BonusCycleSnapshot {
  const packed = wasm.slot_v2_bonus_cycle() >>> 0;

  return {
    activeBefore: (packed & 1) !== 0,
    kind: ((packed >>> 8) & 0xff) as BonusKind,
    outcome: ((packed >>> 16) & 0xff) as BonusCycleOutcome,
  };
}


export interface UpperComebackSnapshot {
  active: boolean;
  lastEnded: boolean;
  lastHit: boolean;
  gamesLeft: number;
  lastGamesBefore: number;
}

export function readUpperComeback(
  wasm: SlotWasmV2,
): UpperComebackSnapshot {
  const packed = wasm.slot_v2_upper_comeback() >>> 0;

  return {
    active: (packed & 1) !== 0,
    lastEnded: (packed & (1 << 1)) !== 0,
    lastHit: (packed & (1 << 2)) !== 0,
    gamesLeft: (packed >>> 8) & 0xff,
    lastGamesBefore: (packed >>> 16) & 0xff,
  };
}


export interface ATSingleTransitionSnapshot {
  applied: boolean;
  regularBonusStarted: boolean;
  episodeBonusStarted: boolean;
  specialStarted: boolean;
}

export function readATSingleTransition(
  wasm: SlotWasmV2,
): ATSingleTransitionSnapshot {
  const packed = wasm.slot_v2_at_single_transition() >>> 0;

  return {
    applied: (packed & 1) !== 0,
    regularBonusStarted: (packed & (1 << 1)) !== 0,
    episodeBonusStarted: (packed & (1 << 2)) !== 0,
    specialStarted: (packed & (1 << 3)) !== 0,
  };
}


export interface NormalATTriggerSnapshot {
  started: boolean;
  guaranteeConsumed: boolean;
  fromBellFive: boolean;
  fromNextHitGuarantee: boolean;
}

export function readNormalATTrigger(
  wasm: SlotWasmV2,
): NormalATTriggerSnapshot {
  const packed = wasm.slot_v2_normal_at_trigger() >>> 0;

  return {
    started: (packed & 1) !== 0,
    guaranteeConsumed: (packed & (1 << 1)) !== 0,
    fromBellFive: (packed & (1 << 2)) !== 0,
    fromNextHitGuarantee: (packed & (1 << 3)) !== 0,
  };
}


export interface BonusTransitionSnapshot {
  outcome: BonusTransitionOutcome;
  returnArea: MachineArea;
}

export function readBonusTransition(
  wasm: SlotWasmV2,
): BonusTransitionSnapshot {
  const packed = wasm.slot_v2_bonus_transition() >>> 0;

  return {
    outcome: (packed & 0xff) as BonusTransitionOutcome,
    returnArea: ((packed >>> 8) & 0xff) as MachineArea,
  };
}


export function readSpecialZoneResult(
  wasm: SlotWasmV2,
): SpecialZoneHitResult {
  return wasm.slot_v2_special_zone_result() as SpecialZoneHitResult;
}


export interface EntryGateSnapshot {
  active: boolean;
  armedThisGame: boolean;
  kind: EntryGateKind;
  target: number;
  queuedStock: number;
}

export function readEntryGate(wasm: SlotWasmV2): EntryGateSnapshot {
  const packed = wasm.slot_v2_entry_gate() >>> 0;
  return {
    active: (packed & 1) !== 0,
    armedThisGame: (packed & (1 << 1)) !== 0,
    kind: ((packed >>> 8) & 0xff) as EntryGateKind,
    target: (packed >>> 16) & 0xff,
    queuedStock: (packed >>> 24) & 0xff,
  };
}

export interface RevivalSnapshot {
  active: boolean;
  kickedNormalHit: boolean;
  gamesLeft: number;
  reviveTier: ATTier;
}

export interface RevivalGameSnapshot {
  active: boolean;
  revivalHit: boolean;
  role: RoleFlag;
  gamesBefore: number;
  gamesAfter: number;
}

export interface RevivalFinalizeSnapshot {
  outcome: RevivalOutcome;
  tier: ATTier;
  stockAdded: boolean;
}

export function readRevivalState(wasm: SlotWasmV2): RevivalSnapshot {
  const packed = wasm.slot_v2_revival_state() >>> 0;

  return {
    active: (packed & 1) !== 0,
    kickedNormalHit: (packed & (1 << 1)) !== 0,
    gamesLeft: (packed >>> 8) & 0xff,
    reviveTier: ((packed >>> 16) & 0xff) as ATTier,
  };
}

export function readRevivalGame(wasm: SlotWasmV2): RevivalGameSnapshot {
  const packed = wasm.slot_v2_revival_game() >>> 0;

  return {
    active: (packed & 1) !== 0,
    revivalHit: (packed & (1 << 1)) !== 0,
    role: ((packed >>> 8) & 0xff) as RoleFlag,
    gamesBefore: (packed >>> 16) & 0xff,
    gamesAfter: (packed >>> 24) & 0xff,
  };
}

export function readRevivalFinalize(
  wasm: SlotWasmV2,
): RevivalFinalizeSnapshot {
  const packed = wasm.slot_v2_revival_finalize() >>> 0;

  return {
    outcome: (packed & 0xff) as RevivalOutcome,
    tier: ((packed >>> 8) & 0xff) as ATTier,
    stockAdded: ((packed >>> 16) & 1) !== 0,
  };
}
