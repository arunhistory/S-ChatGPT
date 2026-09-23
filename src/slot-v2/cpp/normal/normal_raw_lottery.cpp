#include "index.hpp"

namespace slotv2::normal_raw_lottery {

Reward draw(
    Rng& rng,
    uint8_t setting
) {
    const auto p = setting_profile::normalRaw(setting);

    if (rng.next27() < p.at_threshold_27) {
        return Reward::LowerAT;
    }

    if (rng.next27() < p.bonus_threshold_27) {
        return Reward::Bonus;
    }

    if (rng.next27() < p.cz_threshold_27) {
        return Reward::CZ;
    }

    return Reward::None;
}

} // namespace slotv2::normal_raw_lottery
