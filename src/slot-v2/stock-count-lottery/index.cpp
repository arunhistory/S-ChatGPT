#include "index.hpp"

namespace slotv2::stock_count_lottery {

uint8_t fromRoll(uint16_t roll_0_to_999, Profile profile) {
    if (profile == Profile::None) return 0u;

    const uint32_t roll = static_cast<uint32_t>(roll_0_to_999 % 1000u);

    if (profile == Profile::Middle) {
        // 1:800 / 2:150 / 3:40 / 4:9 / 5:1
        if (roll < 800u) return 1u;
        if (roll < 950u) return 2u;
        if (roll < 990u) return 3u;
        if (roll < 999u) return 4u;
        return 5u;
    }

    // Upper
    // 1:600 / 2:250 / 3:100 / 4:45 / 5:5
    if (roll < 600u) return 1u;
    if (roll < 850u) return 2u;
    if (roll < 950u) return 3u;
    if (roll < 995u) return 4u;
    return 5u;
}

uint8_t draw(Rng& rng, Profile profile) {
    if (profile == Profile::None) return 0u;
    return fromRoll(
        static_cast<uint16_t>(rng.uniformBelow(1000u)),
        profile
    );
}

} // namespace slotv2::stock_count_lottery
