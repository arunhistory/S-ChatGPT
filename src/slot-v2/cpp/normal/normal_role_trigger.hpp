#pragma once
#include <stdint.h>
#include "shared/rng.hpp"
#include "shared/types.hpp"
#include "core/machine_state.hpp"
#include "core/pending_event.hpp"
#include "normal/normal_mode.hpp"
#include "normal/normal_at_trigger.hpp"
#include "normal/normal_hit_entry.hpp"

namespace slotv2::normal_role_trigger {

enum class DrawResult : uint8_t {
    None = 0,
    Bonus = 1,
    LowerAT = 2,
    MiddleAT = 3
};

enum class Outcome : uint8_t {
    None = 0,
    BonusQueued = 1,
    LowerATQueued = 2,
    MiddleATQueued = 3
};

struct ApplyResult {
    Outcome outcome{Outcome::None};
    normal_at_trigger::Result guarantee{};
    normal_hit_entry::Result entry{};
};

DrawResult draw(
    Rng& rng,
    RoleFlag role
);

ApplyResult apply(
    machine_state::State& machine,
    pending_event::State& pending,
    normal_mode::Mode mode,
    DrawResult result
);

} // namespace slotv2::normal_role_trigger
