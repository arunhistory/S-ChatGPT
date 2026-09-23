#include "at/at_add_games.hpp"

namespace slotv2::at_add_games {

uint16_t fromRoll(uint16_t roll_0_to_99) {
    const uint16_t roll = static_cast<uint16_t>(roll_0_to_99 % 100u);
    if (roll < 40u) return 10u;
    if (roll < 70u) return 20u;
    if (roll < 88u) return 30u;
    if (roll < 96u) return 50u;
    if (roll < 99u) return 100u;
    return 200u;
}

uint16_t draw(Rng& rng) {
    return fromRoll(
        static_cast<uint16_t>(rng.uniformBelow(100u))
    );
}

} // namespace slotv2::at_add_games
