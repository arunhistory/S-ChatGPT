import {
  readAcquisition,
  readATCycle,
  readATResolution,
  readATSnapshot,
  readATWindow,
  readBellNavigation,
  readCZCycle,
  readCZFinalize,
  readMachineSnapshot,
  readNormalMode,
  readNormalProgress,
  readPendingEvents,
  readSectionReward,
  readSectionTransition,
  readSessionPhase,
  readSpecialResult,
  readSpecialZone,
  SlotWasmV2,
} from "../bridge/index.js";
import {
  AcquisitionResult,
  NormalMode,
  SessionPhase,
  SpecialResult,
} from "../types.js";
import {
  ATCycleSnapshot,
  ATResolutionSnapshot,
  ATSnapshot,
  ATWindowSnapshot,
  BellNavigationSnapshot,
  CZCycleSnapshot,
  CZFinalizeSnapshot,
  MachineSnapshot,
  NormalProgressSnapshot,
  PendingEventSnapshot,
  SectionRewardSnapshot,
  SectionTransitionSnapshot,
  SpecialZoneSnapshot,
} from "../bridge/index.js";

export interface SlotV2Snapshot {
  phase: SessionPhase;
  special: SpecialResult;
  acquisition: AcquisitionResult;
  machine: MachineSnapshot;
  at: ATSnapshot;
  atCycle: ATCycleSnapshot;
  atResolution: ATResolutionSnapshot;
  atWindow: ATWindowSnapshot;
  bellNavigation: BellNavigationSnapshot;
  normalMode: NormalMode;
  normal: NormalProgressSnapshot;
  cz: CZCycleSnapshot;
  czFinalize: CZFinalizeSnapshot;
  pending: PendingEventSnapshot;
  sectionReward: SectionRewardSnapshot;
  sectionTransition: SectionTransitionSnapshot;
  specialZone: SpecialZoneSnapshot;
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
    atWindow: readATWindow(wasm),
    bellNavigation: readBellNavigation(wasm),
    normalMode: readNormalMode(wasm),
    normal: readNormalProgress(wasm),
    cz: readCZCycle(wasm),
    czFinalize: readCZFinalize(wasm),
    pending: readPendingEvents(wasm),
    sectionReward: readSectionReward(wasm),
    sectionTransition: readSectionTransition(wasm),
    specialZone: readSpecialZone(wasm),
  };
}
