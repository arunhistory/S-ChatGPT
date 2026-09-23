#pragma once
#include <stdint.h>
#include "../shared/rng.hpp"
#include "../shared/types.hpp"

namespace slotv2::revival_lottery {

// 0..999の1000分率。
// 指定のない役（1枚役/ハズレ/ペンギン等）は復活抽選なし。
uint16_t threshold(RoleFlag role);
bool fromRoll(RoleFlag role, uint16_t roll_0_to_999);
bool draw(Rng& rng, RoleFlag role);

} // namespace slotv2::revival_lottery
