import { SpecialHit } from "../types.js";
import { playFreeze, FreezePresentationPort } from "./freeze/index.js";

export interface SpecialPresentationPorts {
  freeze?: FreezePresentationPort;
}

// 特殊成立の判定はしない。
// C++から渡された確定SpecialHitを、該当する独立演出へ振り分けるだけ。
export async function playSpecial(
  special: SpecialHit,
  ports: SpecialPresentationPorts,
): Promise<void> {
  switch (special) {
    case SpecialHit.Freeze:
      if (ports.freeze) {
        await playFreeze(special, ports.freeze);
      }
      return;

    case SpecialHit.MiddleATStock:
    case SpecialHit.UpperAT:
    case SpecialHit.None:
    default:
      return;
  }
}
