#include "index.hpp"

namespace slotv2::cz_lottery {

uint16_t thresholdPerThousand(RoleFlag role) {
    switch (role) {
        case RoleFlag::Bell9:
        case RoleFlag::Bell15:
            return 220u; // 22%

        case RoleFlag::Replay:
            return 250u; // 25%

        case RoleFlag::Watermelon:
            return 500u; // 50%

        case RoleFlag::WeakCherry:
        case RoleFlag::WeakChance:
            return 600u; // 60%

        case RoleFlag::StrongChance:
            return 900u; // 90%

        case RoleFlag::StrongCherry:
        case RoleFlag::PenguinChance:
            return 1000u; // breakthrough guaranteed

        case RoleFlag::None:
        case RoleFlag::Miss:
        case RoleFlag::OneMedal:
        default:
            return 0u;
    }
}

bool fromRoll(RoleFlag role, uint16_t roll_0_to_999) {
    const auto threshold = thresholdPerThousand(role);
    if (threshold == 0u) return false;
    return static_cast<uint16_t>(roll_0_to_999 % 1000u) < threshold;
}

bool draw(Rng& rng, RoleFlag role) {
    const auto threshold = thresholdPerThousand(role);
    if (threshold == 0u) return false;
    if (threshold >= 1000u) return true;
    return rng.uniformBelow(1000u) < threshold;
}

} // namespace slotv2::cz_lottery
