import { LeverResult, RoleFlag, SpecialHit } from "../types";

export interface SlotWasmV2 {
  slot_v2_reset(seedLo: number, seedHi: number): void;
  slot_v2_lever(): number;
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

// TSは結果を決めない。C++/WASMが返した確定値を型付きで受け取るだけ。
export function lever(wasm: SlotWasmV2): LeverResult {
  return decodeLeverResult(wasm.slot_v2_lever() >>> 0);
}
