#include "index.hpp"

namespace slotv2::bonus_entry_policy {

Result forMode(normal_mode::Mode mode) {
    switch (mode) {
        case normal_mode::Mode::Heaven:
        case normal_mode::Mode::SuperHeaven:
            return {
                Status::Resolved,
                bonus_state::Kind::Episode
            };

        case normal_mode::Mode::NormalA:
        case normal_mode::Mode::NormalB:
            return {
                Status::Resolved,
                bonus_state::Kind::Regular
            };

        case normal_mode::Mode::Special:
        default:
            return {
                Status::SpecialModeUnresolved,
                bonus_state::Kind::Regular
            };
    }
}

} // namespace slotv2::bonus_entry_policy
