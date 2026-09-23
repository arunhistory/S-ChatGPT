#pragma once
#include "shared/types.hpp"

namespace slotv2::special_lottery {

// 本抽選とは完全に別系統。
// 2^27の1回抽選で 1/8192・1/32768・1/134217728 を同時に判定する。
SpecialHit draw(uint32_t draw27);

} // namespace slotv2::special_lottery
