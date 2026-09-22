#include "index.hpp"

namespace slotv2::stock_restart_lottery {

uint16_t fromRoll(uint16_t roll_0_to_1999) {
    const uint16_t roll = static_cast<uint16_t>(roll_0_to_1999 % 2000u);
    if (roll < 400u) return 20u;   // 20%
    if (roll < 900u) return 30u;   // 25%
    if (roll < 1300u) return 40u;  // 20%
    if (roll < 1600u) return 50u;  // 15%
    if (roll < 1760u) return 75u;  // 8%
    if (roll < 1880u) return 100u; // 6%
    if (roll < 1950u) return 150u; // 3.5%
    return 200u;                   // 2.5%
}

uint16_t draw(Rng& rng) {
    return fromRoll(
        static_cast<uint16_t>(rng.uniformBelow(2000u))
    );
}

} // namespace slotv2::stock_restart_lottery
