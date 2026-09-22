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
  readMachineSnapshot,
  readNormalATTrigger,
  readNormalMode,
  readNormalProgress,
  readPendingEvents,
  readSectionReward,
  readSectionTransition,
  readSessionPhase,
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
  MachineSnapshot,
  NormalATTriggerSnapshot,
  NormalProgressSnapshot,
  PendingEventSnapshot,
  SectionRewardSnapshot,
  SectionTransitionSnapshot,
  SpecialZoneSnapshot,
  UpperComebackSnapshot,
} from "../bridge/index.js";

export interface SlotV2Snapshot {
  phase: SessionPhase;
  special: SpecialResult;
  acquisition: AcquisitionResult;
  machine: MachineSnapshot;
  at: ATSnapshot;
  atCycle: ATCycleSnapshot;
  atResolution: ATResolutionSnapshot;
  atSingleTransition: ATSingleTransitionSnapshot;
  atWindow: ATWindowSnapshot;
  bellNavigation: BellNavigationSnapshot;
  bonus: BonusSnapshot;
  bonusCycle: BonusCycleSnapshot;
  bonusTransition: BonusTransitionSnapshot;
  normalMode: NormalMode;
  normal: NormalProgressSnapshot;
  normalATTrigger: NormalATTriggerSnapshot;
  cz: CZCycleSnapshot;
  czFinalize: CZFinalizeSnapshot;
  pending: PendingEventSnapshot;
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
    phase: readSessionPhase(wasm),
    special: readSpecialResult(wasm),
    acquisition: readAcquisition(wasm),
    machine: readMachineSnapshot(wasm),
    at: readATSnapshot(wasm),
    atCycle: readATCycle(wasm),
    atResolution: readATResolution(wasm),
    atSingleTransition: readATSingleTransition(wasm),
    atWindow: readATWindow(wasm),
    bellNavigation: readBellNavigation(wasm),
    bonus: readBonusState(wasm),
    bonusCycle: readBonusCycle(wasm),
    bonusTransition: readBonusTransition(wasm),
    normalMode: readNormalMode(wasm),
    normal: readNormalProgress(wasm),
    normalATTrigger: readNormalATTrigger(wasm),
    cz: readCZCycle(wasm),
    czFinalize: readCZFinalize(wasm),
    pending: readPendingEvents(wasm),
    sectionReward: readSectionReward(wasm),
    sectionTransition: readSectionTransition(wasm),
    specialZone: readSpecialZone(wasm),
    specialZoneResult: readSpecialZoneResult(wasm),
    upperComeback: readUpperComeback(wasm),
  };
}
