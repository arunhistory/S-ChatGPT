import { SpecialHit } from "../../types";

export interface FreezePresentationPort {
  lockInput(): void;
  blackout(): Promise<void>;
  reverseReels(): Promise<void>;
  showBlue7Target(): Promise<void>;
  unlockInput(): void;
}

// 特殊演出は独立制御。
// 当否・停止結果・内部状態は変更せず、C++で確定したFREEZEを見せるだけ。
// 逆回転はフリーズ専用工程なので、この制御内に含める。
export async function playFreeze(
  special: SpecialHit,
  port: FreezePresentationPort,
): Promise<void> {
  if (special !== SpecialHit.Freeze) return;

  port.lockInput();
  try {
    await port.blackout();
    await port.reverseReels();
    await port.showBlue7Target();
  } finally {
    port.unlockInput();
  }
}
