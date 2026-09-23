#pragma once
#include "../machine-state/index.hpp"
#include "../pending-event/index.hpp"
#include "../upper-comeback-cycle/index.hpp"

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
