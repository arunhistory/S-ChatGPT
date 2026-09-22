#include "index.hpp"

namespace slotv2::normal_shortening {

uint16_t weak(Rng& rng) {
    const uint32_t roll = rng.uniformBelow(100u);
    if (roll < 20u) return 5u;
    if (roll < 45u) return 10u;
    if (roll < 70u) return 15u;
    if (roll < 85u) return 20u;
    if (roll < 95u) return 25u;
    return 50u;
}

uint16_t strong(Rng& rng) {
    const uint32_t roll = rng.uniformBelow(100u);
    if (roll < 15u) return 20u;
    if (roll < 40u) return 50u;
    if (roll < 65u) return 75u;
    if (roll < 85u) return 100u;
    if (roll < 95u) return 150u;
    return 200u;
}

uint16_t continuous(Rng& rng) {
    static constexpr uint16_t kGames[6] = {
        5u, 20u, 40u, 60u, 80u, 100u
    };

    uint32_t total = 0u;
    bool again = true;

    while (again) {
        total += kGames[rng.uniformBelow(6u)];
        again = false;

        for (uint8_t i = 0u; i < 10u; ++i) {
            if (rng.oneIn(15u)) {
                again = true;
                break;
            }
        }

        if (total >= 65535u) return 65535u;
    }

    return static_cast<uint16_t>(total);
}

} // namespace slotv2::normal_shortening
