#pragma once
#include <stdint.h>
#include "shared/rng.hpp"

namespace slotv2::stock_count_lottery {

enum class Profile : uint8_t {
    None = 0,
    Middle = 1,
    Upper = 2
};

// Middle:
// 1=80% / 2=15% / 3=4% / 4=0.9% / 5=0.1%
//
// Upper:
// 1=60% / 2=25% / 3=10% / 4=4.5% / 5=0.5%
uint8_t fromRoll(uint16_t roll_0_to_999, Profile profile);
uint8_t draw(Rng& rng, Profile profile);

} // namespace slotv2::stock_count_lottery
