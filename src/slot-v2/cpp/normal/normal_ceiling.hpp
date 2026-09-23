#pragma once
#include <stdint.h>
#include "shared/rng.hpp"
#include "normal/normal_mode.hpp"

namespace slotv2::normal_ceiling {

enum class Reward : uint8_t {
    None = 0,
    CZ = 1,
    Bonus = 2,
    LowerAT = 3,
    LowerATWithStock = 4,
    Freeze = 5
};

Reward fromRoll(
    normal_mode::Mode mode,
    uint16_t ceiling,
    uint8_t roll_0_to_99
);

Reward draw(
    Rng& rng,
    normal_mode::Mode mode,
    uint16_t ceiling
);

} // namespace slotv2::normal_ceiling
