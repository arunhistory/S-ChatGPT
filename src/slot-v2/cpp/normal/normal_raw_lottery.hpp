#pragma once
#include <stdint.h>
#include "shared/rng.hpp"
#include "core/setting_profile.hpp"

namespace slotv2::normal_raw_lottery {

enum class Reward : uint8_t {
    None = 0,
    LowerAT = 1,
    Bonus = 2,
    CZ = 3
};

Reward draw(
    Rng& rng,
    uint8_t setting
);

} // namespace slotv2::normal_raw_lottery
