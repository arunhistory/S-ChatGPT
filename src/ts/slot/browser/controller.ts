import {
  completeSpecial,
  lever,
  pushLowerFallChallenge,
  resetWithSetting,
  SlotWasmV2,
  stop,
} from "../bridge/index.js";
import {
  LeverResult,
  LowerFallPushOutcome,
  ReelId,
  ReelPosition,
  SessionPhase,
  SettingResetStatus,
  SlotSetting,
  StopResult,
} from "../types.js";
import {
  readSlotV2Snapshot,
  SlotV2Snapshot,
} from "./snapshot.js";

export interface LeverCommandResult {
  result: LeverResult;
  snapshot: SlotV2Snapshot;
}

export interface StopCommandResult {
  result: StopResult;
  snapshot: SlotV2Snapshot;
}

export interface SpecialCompleteResult {
  phase: SessionPhase;
  snapshot: SlotV2Snapshot;
}

export interface LowerFallPushResult {
  outcome: LowerFallPushOutcome;
  snapshot: SlotV2Snapshot;
}

export interface SettingChangeResult {
  status: SettingResetStatus;
  snapshot: SlotV2Snapshot;
}

function randomSeed64(): [number, number] {
  const words = new Uint32Array(2);
  crypto.getRandomValues(words);
  return [words[0] >>> 0, words[1] >>> 0];
}

// ブラウザ入力の窓口。
// 抽選や状態変更をTSに持たせず、必ずWASMへ命令してからsnapshotを読み直す。
export class SlotV2Controller {
  constructor(private readonly wasm: SlotWasmV2) {}

  changeSetting(setting: SlotSetting): SettingChangeResult {
    const [seedLo, seedHi] = randomSeed64();
    const status = resetWithSetting(
      this.wasm,
      seedLo,
      seedHi,
      setting,
    );

    return {
      status,
      snapshot: readSlotV2Snapshot(this.wasm),
    };
  }

  pullLever(): LeverCommandResult {
    const result = lever(this.wasm);

    return {
      result,
      snapshot: readSlotV2Snapshot(this.wasm),
    };
  }

  stopReel(
    reel: ReelId,
    pressedPosition: ReelPosition,
  ): StopCommandResult {
    const result = stop(
      this.wasm,
      reel,
      pressedPosition,
    );

    return {
      result,
      snapshot: readSlotV2Snapshot(this.wasm),
    };
  }

  finishSpecial(): SpecialCompleteResult {
    const phase = completeSpecial(this.wasm);

    return {
      phase,
      snapshot: readSlotV2Snapshot(this.wasm),
    };
  }

  pushLowerFall(): LowerFallPushResult {
    const outcome = pushLowerFallChallenge(this.wasm);

    return {
      outcome,
      snapshot: readSlotV2Snapshot(this.wasm),
    };
  }

  snapshot(): SlotV2Snapshot {
    return readSlotV2Snapshot(this.wasm);
  }
}
