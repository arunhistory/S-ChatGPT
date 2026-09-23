import {
  readAcquisition,
  readATCycle,
  readATResolution,
  readATSnapshot,
  readATSingleTransition,
  readATWindow,
  readBellNavigation,
  readBonusCycle,
  readBonusState,
  readBonusTransition,
  readCZCycle,
  readCZFinalize,
  readEntryGate,
  readMachineSnapshot,
  readLowerFallChallenge,
  readNormalATTrigger,
  readNormalMode,
  readNormalRoute,
  readNormalCeilingTransition,
  readNormalProgress,
  readPendingEvents,
  readRevivalFinalize,
  readRevivalGame,
  readRevivalState,
  readSectionReward,
  readSectionTransition,
  readSessionPhase,
  readSetting,
  readSpecialResult,
  readSpecialZone,
  readSpecialZoneResult,
  readUpperComeback,
  SlotWasmV2,
} from "../bridge/index.js";
import {
  AcquisitionResult,
  NormalMode,
  SessionPhase,
  SlotSetting,
  SpecialResult,
  SpecialZoneHitResult,
} from "../types.js";
import {
  ATCycleSnapshot,
  ATResolutionSnapshot,
  ATSnapshot,
  ATSingleTransitionSnapshot,
  ATWindowSnapshot,
  BellNavigationSnapshot,
  BonusCycleSnapshot,
  BonusSnapshot,
  BonusTransitionSnapshot,
  CZCycleSnapshot,
  CZFinalizeSnapshot,
  EntryGateSnapshot,
  MachineSnapshot,
  LowerFallChallengeSnapshot,
  NormalATTriggerSnapshot,
  NormalProgressSnapshot,
  NormalRouteSnapshot,
  PendingEventSnapshot,
  RevivalFinalizeSnapshot,
  RevivalGameSnapshot,
  RevivalSnapshot,
  SectionRewardSnapshot,
  SectionTransitionSnapshot,
  SpecialZoneSnapshot,
  UpperComebackSnapshot,
} from "../bridge/index.js";

export interface SlotV2Snapshot {
  setting: SlotSetting;
  phase: SessionPhase;
  special: SpecialResult;
  acquisition: AcquisitionResult;
  machine: MachineSnapshot;
  at: ATSnapshot;
  atCycle: ATCycleSnapshot;
  atResolution: ATResolutionSnapshot;
  atSingleTransition: ATSingleTransitionSnapshot;
  atWindow: ATWindowSnapshot;
  lowerFallChallenge: LowerFallChallengeSnapshot;
  bellNavigation: BellNavigationSnapshot;
  bonus: BonusSnapshot;
  bonusCycle: BonusCycleSnapshot;
  bonusTransition: BonusTransitionSnapshot;
  normalMode: NormalMode;
  normalRoute: NormalRouteSnapshot;
  normalCeilingTransition: number;
  normal: NormalProgressSnapshot;
  normalATTrigger: NormalATTriggerSnapshot;
  cz: CZCycleSnapshot;
  czFinalize: CZFinalizeSnapshot;
  entryGate: EntryGateSnapshot;
  pending: PendingEventSnapshot;
  revival: RevivalSnapshot;
  revivalGame: RevivalGameSnapshot;
  revivalFinalize: RevivalFinalizeSnapshot;
  sectionReward: SectionRewardSnapshot;
  sectionTransition: SectionTransitionSnapshot;
  specialZone: SpecialZoneSnapshot;
  specialZoneResult: SpecialZoneHitResult;
  upperComeback: UpperComebackSnapshot;
}

// UIはこのsnapshotを描画するだけ。
// 状態遷移・抽選・払出判定は一切ここで行わない。
export function readSlotV2Snapshot(
  wasm: SlotWasmV2,
): SlotV2Snapshot {
  return {
    setting: readSetting(wasm),
    phase: readSessionPhase(wasm),
    special: readSpecialResult(wasm),
    acquisition: readAcquisition(wasm),
    machine: readMachineSnapshot(wasm),
    at: readATSnapshot(wasm),
    atCycle: readATCycle(wasm),
    atResolution: readATResolution(wasm),
    atSingleTransition: readATSingleTransition(wasm),
    atWindow: readATWindow(wasm),
    lowerFallChallenge: readLowerFallChallenge(wasm),
    bellNavigation: readBellNavigation(wasm),
    bonus: readBonusState(wasm),
    bonusCycle: readBonusCycle(wasm),
    bonusTransition: readBonusTransition(wasm),
    normalMode: readNormalMode(wasm),
    normalRoute: readNormalRoute(wasm),
    normalCeilingTransition: readNormalCeilingTransition(wasm),
    normal: readNormalProgress(wasm),
    normalATTrigger: readNormalATTrigger(wasm),
    cz: readCZCycle(wasm),
    czFinalize: readCZFinalize(wasm),
    entryGate: readEntryGate(wasm),
    pending: readPendingEvents(wasm),
    revival: readRevivalState(wasm),
    revivalGame: readRevivalGame(wasm),
    revivalFinalize: readRevivalFinalize(wasm),
    sectionReward: readSectionReward(wasm),
    sectionTransition: readSectionTransition(wasm),
    specialZone: readSpecialZone(wasm),
    specialZoneResult: readSpecialZoneResult(wasm),
    upperComeback: readUpperComeback(wasm),
  };
}
