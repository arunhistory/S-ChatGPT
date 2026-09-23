#pragma once
#include <stdint.h>
#include "shared/rng.hpp"

namespace slotv2::special_ceiling {

enum class Ceiling : uint16_t {
    G777 = 777,
    G1500 = 1500
};

enum class Reward : uint8_t {
    LowerATWithStock = 0,
    Freeze = 1
};

struct Result {
    Ceiling ceiling{Ceiling::G777};
    Reward reward{Reward::LowerATWithStock};
    bool freeze{false};
};

// Special mode:
// 777G 95% -> lower AT + stock.
// 1500G 5% -> freeze guaranteed.
Result fromRoll(uint16_t roll_0_to_999);
Result draw(Rng& rng);

} // namespace slotv2::special_ceiling
