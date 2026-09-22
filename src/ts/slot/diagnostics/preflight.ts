import { SlotWasmV2 } from "../bridge/index.js";

export interface PreflightReport {
  leftDefined: boolean;
  middleDefined: boolean;
  rightDefined: boolean;

  leftCherryHide: boolean;
  leftBellAssist: boolean;
  leftReplayAssist: boolean;
  leftBarLandmarks: boolean;

  middleBellAssist: boolean;
  middleReplayAssist: boolean;

  rightBellAssist: boolean;
  rightReplayAssist: boolean;

  playable: boolean;
  rawBits: number;
}

export function readPreflight(wasm: SlotWasmV2): PreflightReport {
  const bits = wasm.slot_v2_preflight() >>> 0;

  return {
    leftDefined: (bits & (1 << 0)) !== 0,
    middleDefined: (bits & (1 << 1)) !== 0,
    rightDefined: (bits & (1 << 2)) !== 0,

    leftCherryHide: (bits & (1 << 3)) !== 0,
    leftBellAssist: (bits & (1 << 4)) !== 0,
    leftReplayAssist: (bits & (1 << 5)) !== 0,
    leftBarLandmarks: (bits & (1 << 6)) !== 0,

    middleBellAssist: (bits & (1 << 7)) !== 0,
    middleReplayAssist: (bits & (1 << 8)) !== 0,

    rightBellAssist: (bits & (1 << 9)) !== 0,
    rightReplayAssist: (bits & (1 << 10)) !== 0,

    playable: (bits & 0x80000000) !== 0,
    rawBits: bits,
  };
}

export function missingPreflightConditions(report: PreflightReport): string[] {
  const missing: string[] = [];

  if (!report.leftDefined) missing.push("left-strip");
  if (!report.middleDefined) missing.push("middle-strip");
  if (!report.rightDefined) missing.push("right-strip");

  if (!report.leftCherryHide) missing.push("left-cherry-hide");
  if (!report.leftBellAssist) missing.push("left-bell-assist");
  if (!report.leftReplayAssist) missing.push("left-replay-assist");
  if (!report.leftBarLandmarks) missing.push("left-bar-landmarks");

  if (!report.middleBellAssist) missing.push("middle-bell-assist");
  if (!report.middleReplayAssist) missing.push("middle-replay-assist");

  if (!report.rightBellAssist) missing.push("right-bell-assist");
  if (!report.rightReplayAssist) missing.push("right-replay-assist");

  return missing;
}
