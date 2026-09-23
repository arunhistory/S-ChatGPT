#include "bonus/bonus_upgrade.hpp"

namespace slotv2::bonus_upgrade {

bool toEpisode(Rng& rng) {
    return rng.oneIn(100u);
}

} // namespace slotv2::bonus_upgrade
