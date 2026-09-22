import {
  readVisibleSymbol,
  SlotWasmV2,
} from "../bridge/index.js";
import {
  ReelId,
  ReelPosition,
  Symbol,
} from "../types.js";

export function symbolText(symbol: Symbol): string {
  switch (symbol) {
    case Symbol.Red7:
      return "🟥7";
    case Symbol.Blue7:
      return "🟦7";
    case Symbol.Bar:
      return "BAR";
    case Symbol.Bell:
      return "🔔";
    case Symbol.Replay:
      return "REPLAY";
    case Symbol.Cherry:
      return "🍒";
    case Symbol.Watermelon:
      return "🍉";
    case Symbol.Penguin:
      return "🐧";
    case Symbol.Snow:
      return "❄️";
    case Symbol.Unknown:
    default:
      return "?";
  }
}

export interface VisibleReelText {
  top: string;
  center: string;
  bottom: string;
}

export function readVisibleReelText(
  wasm: SlotWasmV2,
  reel: ReelId,
  centerPosition: ReelPosition,
): VisibleReelText {
  return {
    top: symbolText(
      readVisibleSymbol(wasm, reel, centerPosition, -1),
    ),
    center: symbolText(
      readVisibleSymbol(wasm, reel, centerPosition, 0),
    ),
    bottom: symbolText(
      readVisibleSymbol(wasm, reel, centerPosition, 1),
    ),
  };
}
