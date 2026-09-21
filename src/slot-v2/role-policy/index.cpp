#include "index.hpp"

namespace slotv2::role_policy {

StopPolicy stopPolicy(RoleFlag role) {
    switch (role) {
        case RoleFlag::Bell9:
        case RoleFlag::Bell15:
        case RoleFlag::Replay:
            return StopPolicy::Assist;

        case RoleFlag::WeakCherry:
        case RoleFlag::StrongCherry:
            return StopPolicy::EyeStop;

        case RoleFlag::Watermelon:
        case RoleFlag::WeakChance:
        case RoleFlag::StrongChance:
        case RoleFlag::PenguinChance:
            return StopPolicy::SubstituteCapable;

        case RoleFlag::None:
        case RoleFlag::Miss:
        case RoleFlag::OneMedal:
        default:
            return StopPolicy::Plain;
    }
}

} // namespace slotv2::role_policy
