#include "index.hpp"

namespace slotv2::normal_ceiling {

Reward fromRoll(
    normal_mode::Mode mode,
    uint16_t ceiling,
    uint8_t roll_0_to_99
) {
    if (mode == normal_mode::Mode::Special) {
        if (ceiling == 777u) return Reward::LowerATWithStock;
        if (ceiling == 1500u) return Reward::Freeze;
        return Reward::None;
    }

    const uint8_t roll = static_cast<uint8_t>(roll_0_to_99 % 100u);
    if (roll < 70u) return Reward::CZ;
    if (roll < 95u) return Reward::Bonus;
    return Reward::LowerAT;
}

Reward draw(
    Rng& rng,
    normal_mode::Mode mode,
    uint16_t ceiling
) {
    return fromRoll(
        mode,
        ceiling,
        static_cast<uint8_t>(rng.uniformBelow(100u))
    );
}

} // namespace slotv2::normal_ceiling
