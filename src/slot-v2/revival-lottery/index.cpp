#include "index.hpp"

namespace slotv2::revival_lottery {

uint16_t threshold(RoleFlag role) {
    switch (role) {
        case RoleFlag::Bell9:
        case RoleFlag::Bell15:
            return 400u; // 40.0%

        case RoleFlag::Replay:
            return 601u; // 60.1%

        case RoleFlag::WeakChance:
            return 650u; // 65.0%

        case RoleFlag::Watermelon:
            return 700u; // 70.0%

        case RoleFlag::StrongChance:
            return 800u; // 80.0%

        case RoleFlag::WeakCherry:
            return 850u; // 85.0%

        case RoleFlag::StrongCherry:
            return 999u; // 99.9%

        case RoleFlag::None:
        case RoleFlag::Miss:
        case RoleFlag::OneMedal:
        case RoleFlag::PenguinChance:
        default:
            return 0u;
    }
}

bool fromRoll(RoleFlag role, uint16_t roll_0_to_999) {
    const uint16_t t = threshold(role);
    if (t == 0u) return false;
    return static_cast<uint16_t>(roll_0_to_999 % 1000u) < t;
}

bool draw(Rng& rng, RoleFlag role) {
    const uint16_t t = threshold(role);
    if (t == 0u) return false;
    return rng.uniformBelow(1000u) < t;
}

} // namespace slotv2::revival_lottery
