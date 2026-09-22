import { SlotWasmV2, stop } from "../bridge/index.js";
import { ReelId, ReelPosition, StopResult } from "../types.js";

// STOP入力は「どのリールを、どの位置で押したか」だけC++へ渡す。
// 停止位置の補正・役判定はTS側では行わない。
export function onStop(
  wasm: SlotWasmV2,
  reel: ReelId,
  pressedPosition: ReelPosition,
): StopResult {
  return stop(wasm, reel, pressedPosition);
}
