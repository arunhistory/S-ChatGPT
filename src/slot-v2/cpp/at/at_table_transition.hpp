#pragma once
#include <stdint.h>
#include "shared/rng.hpp"
#include "at/at_state.hpp"

namespace slotv2::at_table_transition {

at_state::Table fromRoll(
    at_state::Table current,
    uint16_t roll_0_to_99
);

at_state::Table draw(
    Rng& rng,
    at_state::Table current
);

} // namespace slotv2::at_table_transition
