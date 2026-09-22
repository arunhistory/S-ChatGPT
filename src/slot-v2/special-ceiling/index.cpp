#include "index.hpp"

namespace slotv2::special_ceiling {

Result draw(Rng& rng) {
    const uint32_t ceiling_roll = rng.uniformBelow(1000u);

    if (ceiling_roll >= 950u) {
        return {
            Ceiling::G1500,
            G777Result::PlainAT,
            true
        };
    }

    const uint32_t result_roll = rng.uniformBelow(3u);
    return {
        Ceiling::G777,
        static_cast<G777Result>(result_roll),
        false
    };
}

} // namespace slotv2::special_ceiling
