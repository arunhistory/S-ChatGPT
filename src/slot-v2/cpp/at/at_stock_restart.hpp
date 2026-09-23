#pragma once
#include <stdint.h>
#include "shared/rng.hpp"
#include "core/machine_state.hpp"
#include "core/pending_event.hpp"

namespace slotv2::at_stock_restart {

struct Result {
    bool applied{false};
    int games{0};
    at_state::Table before_table{at_state::Table::Normal};
    at_state::Table after_table{at_state::Table::Normal};
    uint32_t stock_after{0};
};

int gamesFromRoll(uint16_t roll_0_to_999);

Result apply(
    Rng& rng,
    machine_state::State& machine,
    pending_event::State& pending
);

} // namespace slotv2::at_stock_restart
