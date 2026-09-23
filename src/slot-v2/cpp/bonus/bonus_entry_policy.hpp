#pragma once
#include "../normal-mode/index.hpp"
#include "../bonus-state/index.hpp"

namespace slotv2::bonus_entry_policy {

struct Result {
    bonus_state::Kind kind{bonus_state::Kind::Regular};
};

// Heaven/SuperHeaven -> Episode.
// NormalA/NormalB/Special -> Regular BONUS.
Result forMode(normal_mode::Mode mode);

} // namespace slotv2::bonus_entry_policy
