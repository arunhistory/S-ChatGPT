#pragma once
#include <stdint.h>
#include "shared/rng.hpp"
#include "core/machine_state.hpp"
#include "core/pending_event.hpp"

namespace slotv2::at_internal_transition {

struct Result {
    bool applied{false};
    bool add_games_applied{false};
    uint16_t added_games{0};
    bool fall_applied{false};
    bool at_end_pending{false};
    at_state::Tier before{at_state::Tier::Lower};
    at_state::Tier after{at_state::Tier::Lower};
};

Result apply(
    Rng& rng,
    machine_state::State& machine,
    pending_event::State& pending
);

} // namespace slotv2::at_internal_transition
