#include "index.hpp"

namespace slotv2::cz_lottery {

bool drawBase(Rng& rng) {
    return rng.oneIn(kBaseDenominator);
}

} // namespace slotv2::cz_lottery
