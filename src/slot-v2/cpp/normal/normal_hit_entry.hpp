#pragma once
#include "core/machine_state.hpp"
#include "normal/normal_mode.hpp"
#include "bonus/bonus_entry_policy.hpp"

namespace slotv2::normal_hit_entry {

enum class Outcome : uint8_t {
    None = 0,
    Bonus = 1,
    LowerAT = 2
};

struct Result {
    Outcome outcome{Outcome::None};
    bonus_state::Kind bonus_kind{bonus_state::Kind::Regular};
};

// NormalA/B/Special=regular BONUS, Heaven/SuperHeaven=Episode.
// The hit is queued; actual start waits for the RED symbol entry signal.
Result enterBonus(
    machine_state::State& machine,
    normal_mode::Mode mode
);

// Normal AT is queued as Lower tier. Actual start waits for RED777.
Result enterAT(machine_state::State& machine);

} // namespace slotv2::normal_hit_entry
