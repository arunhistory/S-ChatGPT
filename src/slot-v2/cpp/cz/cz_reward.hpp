#pragma once
#include "shared/rng.hpp"
#include "core/machine_state.hpp"
#include "core/pending_event.hpp"
#include "normal/normal_mode.hpp"

namespace slotv2::cz_reward {

enum class Target : uint8_t {
    NormalHit = 0,
    LowerAT = 1
};

enum class Outcome : uint8_t {
    None = 0,
    NormalHit = 1,
    LowerAT = 2
};

struct Result {
    Outcome outcome{Outcome::None};
    bool from_three_miss{false};
    bool next_hit_guarantee_consumed{false};
};

Target targetFromRoll(uint16_t roll_0_to_9);
Target drawTarget(Rng& rng);

// Resolve CZHit / CZThreeMissHit immediately inside C++.
// Base split: normal hit 70% / lower AT 30%.
Result apply(
    Rng& rng,
    machine_state::State& machine,
    pending_event::State& pending,
    normal_mode::Mode mode
);

} // namespace slotv2::cz_reward
