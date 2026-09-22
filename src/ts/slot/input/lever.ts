import { lever, SlotWasmV2 } from "../bridge/index.js";
import { LeverResult } from "../types.js";

// 入力層はWASMを呼ぶだけ。抽選・役決定・状態変更はTSで行わない。
export function onLever(wasm: SlotWasmV2): LeverResult {
  return lever(wasm);
}
