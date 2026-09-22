import { SlotWasmV2 } from "../bridge/index.js";
import {
  missingPreflightConditions,
  PreflightReport,
  readPreflight,
} from "../diagnostics/preflight.js";
import { loadSlotV2Wasm } from "../wasm/load.js";

export interface SlotV2Boot {
  wasm: SlotWasmV2;
  preflight: PreflightReport;
  missing: string[];
}

export async function initializeSlotV2(
  wasmUrl = "./slot-v2.wasm",
): Promise<SlotV2Boot> {
  const wasm = await loadSlotV2Wasm(wasmUrl);
  const preflight = readPreflight(wasm);

  return {
    wasm,
    preflight,
    missing: missingPreflightConditions(preflight),
  };
}

export function assertSlotV2Playable(boot: SlotV2Boot): void {
  if (boot.preflight.playable) return;

  throw new Error(
    `slot-v2 is not playable yet: ${boot.missing.join(", ")}`,
  );
}
