#include "index.hpp"

namespace slotv2::stock_lottery {

bool onHit(Rng& rng) {
    return rng.oneIn(10u);
}

} // namespace slotv2::stock_lottery
