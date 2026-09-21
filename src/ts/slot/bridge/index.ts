import {
  AcquisitionResult,
  AcquisitionStatus,
  CommandStatus,
  EntryTarget,
  LeverResult,
  ReelId,
  ReelPosition,
  RoleFlag,
  SessionPhase,
  SpecialHit,
  SpecialResult,
  StopResult,
  StopStatus,
  Symbol,
} from "../types";

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
  slot_v2_validate_reel(reel: number): number;
  slot_v2_reel_ready_mask(): number;
  slot_v2_last_special(): number;
  slot_v2_last_role(): number;
  slot_v2_freeze_active(): number;
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
    stock: (packed >>> 16) & 0xff,
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
