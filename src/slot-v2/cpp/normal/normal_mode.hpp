#pragma once
#include <stdint.h>
#include "../shared/rng.hpp"

namespace slotv2::normal_mode {

enum class Mode : uint8_t {
    NormalA = 0,
    NormalB = 1,
    Heaven = 2,
    SuperHeaven = 3,
    Special = 4
};

// 基本モード振分: A45% / B35% / 天国15% / 超天国5%。
// Specialへの移行契機は別制御なので、この抽選には含めない。
Mode drawBase(Rng& rng);

} // namespace slotv2::normal_mode
