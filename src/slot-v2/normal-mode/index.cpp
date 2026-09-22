#include "index.hpp"

namespace slotv2::normal_mode {

Mode drawBase(Rng& rng) {
    const uint32_t roll = rng.uniformBelow(100u);

    if (roll < 45u) return Mode::NormalA;
    if (roll < 80u) return Mode::NormalB;
    if (roll < 95u) return Mode::Heaven;
    return Mode::SuperHeaven;
}

} // namespace slotv2::normal_mode
