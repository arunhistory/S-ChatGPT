import { SlotWasmV2 } from "../bridge/index.js";

const REQUIRED_EXPORTS = [
  "slot_v2_reset",
  "slot_v2_lever",
  "slot_v2_stop",
  "slot_v2_phase",
  "slot_v2_special_result",
  "slot_v2_complete_special",
  "slot_v2_stop_sequence",
  "slot_v2_stopped_position",
  "slot_v2_acquisition",
  "slot_v2_symbol_at",
  "slot_v2_visible_symbol",
  "slot_v2_validate_left",
  "slot_v2_validate_reel",
  "slot_v2_assist_failure_mask",
  "slot_v2_reel_ready_mask",
  "slot_v2_preflight",
  "slot_v2_last_special",
  "slot_v2_last_role",
  "slot_v2_freeze_active",
  "slot_v2_machine_area",
  "slot_v2_at_active",
  "slot_v2_at_tier",
  "slot_v2_at_games_left",
  "slot_v2_special_committed",
  "slot_v2_section_diff",
  "slot_v2_section_minimum",
  "slot_v2_section_count",
  "slot_v2_stock_count",
  "slot_v2_point_count",
  "slot_v2_pending_events",
  "slot_v2_section_reward",
  "slot_v2_at_window",
  "slot_v2_section_transition",
  "slot_v2_special_zone",
  "slot_v2_special_zone_result",
  "slot_v2_bonus_cycle",
  "slot_v2_bonus_transition",
  "slot_v2_upper_comeback",
  "slot_v2_at_single_transition",
  "slot_v2_normal_at_trigger",
  "slot_v2_revival_finalize",
  "slot_v2_revival_game",
  "slot_v2_revival_state",
  "slot_v2_bonus_state",
  "slot_v2_normal_mode",
  "slot_v2_ceiling_at",
  "slot_v2_ceiling_count",
  "slot_v2_cz_cycle",
  "slot_v2_cz_finalize",
  "slot_v2_normal_progress",
  "slot_v2_normal_display_games",
  "slot_v2_normal_actual_games",
  "slot_v2_at_resolution",
  "slot_v2_at_cycle",
  "slot_v2_bell_navigation_correct",
  "slot_v2_bell_navigation_next",
  "slot_v2_bell_navigation",
] as const;

function asSlotWasmV2(exports: WebAssembly.Exports): SlotWasmV2 {
  for (const name of REQUIRED_EXPORTS) {
    if (typeof exports[name] !== "function") {
      throw new Error(`slot-v2.wasm export missing: ${name}`);
    }
  }

  return exports as unknown as SlotWasmV2;
}

async function instantiate(url: string): Promise<WebAssembly.Instance> {
  const response = await fetch(url, { cache: "no-store" });
  if (!response.ok) {
    throw new Error(`slot-v2.wasm fetch failed: ${response.status}`);
  }

  if (typeof WebAssembly.instantiateStreaming === "function") {
    try {
      const result = await WebAssembly.instantiateStreaming(
        Promise.resolve(response.clone()),
        {},
      );
      return result.instance;
    } catch {
      // MIME設定がapplication/wasmでない配信先でも動くようarrayBufferへfallback。
    }
  }

  const bytes = await response.arrayBuffer();
  const result = await WebAssembly.instantiate(bytes, {});
  return result.instance;
}

function randomSeed64(): [number, number] {
  const words = new Uint32Array(2);
  crypto.getRandomValues(words);
  return [words[0] >>> 0, words[1] >>> 0];
}

export async function loadSlotV2Wasm(url: string): Promise<SlotWasmV2> {
  const instance = await instantiate(url);
  const wasm = asSlotWasmV2(instance.exports);

  const [seedLo, seedHi] = randomSeed64();
  wasm.slot_v2_reset(seedLo, seedHi);

  return wasm;
}
