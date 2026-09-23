#pragma once
#include "core/machine_state.hpp"
#include "core/pending_event.hpp"

namespace slotv2::upper_special_transition {

// Starts the upper special zone from either the AT-internal event or
// the section reward event. Returns true when a pending trigger was consumed.
bool apply(
    machine_state::State& machine,
    pending_event::State& pending
);

} // namespace slotv2::upper_special_transition
