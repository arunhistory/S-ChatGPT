import { lever, SlotWasmV2 } from "../bridge";
import { LeverResult } from "../types";

// 入力層はWASMを呼ぶだけ。抽選・役決定・状態変更はTSで行わない。
export function onLever(wasm: SlotWasmV2): LeverResult {
  return lever(wasm);
}
