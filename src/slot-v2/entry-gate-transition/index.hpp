#pragma once
#include <stdint.h>
#include "../machine-state/index.hpp"

namespace slotv2::entry_gate_transition {

enum class Outcome : uint8_t {
    None = 0,
    BonusStarted = 1,
    ATStarted = 2
};

struct Result {
    Outcome outcome{Outcome::None};
    uint32_t stock_added{0};
};

Result apply(
    machine_state::State& machine,
    uint8_t left_pos,
    uint8_t middle_pos,
    uint8_t right_pos
);

} // namespace slotv2::entry_gate_transition
