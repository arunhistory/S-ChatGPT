import {
  LeverResult,
  ReelId,
  ReelPosition,
  RoleFlag,
  SpecialHit,
  StopResult,
  StopStatus,
} from "../types";

export interface SlotWasmV2 {
  slot_v2_reset(seedLo: number, seedHi: number): void;
  slot_v2_lever(): number;
  slot_v2_stop(reel: number, pressedPosition: number): number;
  slot_v2_stopped_position(reel: number): number;
  slot_v2_last_special(): number;
  slot_v2_last_role(): number;
  slot_v2_freeze_active(): number;
}

export function decodeLeverResult(packed: number): LeverResult {
  return {
    role: (packed & 0xff) as RoleFlag,
    special: ((packed >>> 8) & 0xff) as SpecialHit,
    mainLotteryRan: ((packed >>> 16) & 1) === 1,
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
