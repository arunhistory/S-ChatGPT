#pragma once
#include <stdint.h>
#include "../shared/rng.hpp"
#include "../machine-state/index.hpp"
#include "../pending-event/index.hpp"

namespace slotv2::at_window_transition {

enum class Outcome : uint8_t {
    None = 0,
    StockRestart = 1,
    UpperComeback = 2,
    Revival = 3
};

struct Result {
    Outcome outcome{Outcome::None};
    at_state::Tier tier{at_state::Tier::Lower};
    uint16_t restart_games{0};
};

Result apply(
    Rng& rng,
    machine_state::State& machine,
    pending_event::State& pending
);

} // namespace slotv2::at_window_transition
