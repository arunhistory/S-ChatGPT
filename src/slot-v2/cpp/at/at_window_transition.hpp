#pragma once
#include <stdint.h>
#include "../shared/rng.hpp"
#include "../machine-state/index.hpp"
#include "../pending-event/index.hpp"

namespace slotv2::at_window_transition {

enum class Outcome : uint8_t {
    None = 0,
    UpperComeback = 1,
    Revival = 2
};

struct Result {
    Outcome outcome{Outcome::None};
    at_state::Tier tier{at_state::Tier::Lower};
};

// Runs only after at-stock-restart had the first chance to consume a stock.
// With no stock left, Upper goes to the 64G comeback window;
// Lower/Middle go directly to the five-game revival challenge.
Result apply(
    Rng& rng,
    machine_state::State& machine,
    pending_event::State& pending
);

} // namespace slotv2::at_window_transition
