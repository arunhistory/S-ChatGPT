#pragma once
#include <stdint.h>
#include "../machine-state/index.hpp"
#include "../pending-event/index.hpp"

namespace slotv2::special_zone_transition {

enum class Outcome : uint8_t {
    None = 0,
    AddGamesApplied = 1,
    BonusOmenStarted = 2
};

struct Result {
    Outcome outcome{Outcome::None};
    uint16_t added_games{0};
};

Result apply(
    machine_state::State& machine,
    pending_event::State& pending
);

} // namespace slotv2::special_zone_transition
