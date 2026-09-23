#pragma once
#include "core/machine_state.hpp"
#include "core/pending_event.hpp"
#include "at/upper_comeback_cycle.hpp"

namespace slotv2::upper_comeback_transition {

enum class Outcome : uint8_t {
    None = 0,
    UpperAT = 1,
    Revival = 2
};

struct Result {
    Outcome outcome{Outcome::None};
};

Result apply(
    machine_state::State& machine,
    pending_event::State& pending,
    const upper_comeback_cycle::Result& cycle
);

} // namespace slotv2::upper_comeback_transition
