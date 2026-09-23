#include "index.hpp"

namespace slotv2::special_ceiling {

Result fromRoll(uint16_t roll_0_to_999) {
    const uint16_t roll = static_cast<uint16_t>(roll_0_to_999 % 1000u);

    if (roll >= 950u) {
        return {
            Ceiling::G1500,
            Reward::Freeze,
            true
        };
    }

    return {
        Ceiling::G777,
        Reward::LowerATWithStock,
        false
    };
}

Result draw(Rng& rng) {
    return fromRoll(
        static_cast<uint16_t>(rng.uniformBelow(1000u))
    );
}

} // namespace slotv2::special_ceiling
