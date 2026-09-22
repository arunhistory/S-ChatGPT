#pragma once
#include <stdint.h>
#include "../shared/rng.hpp"
#include "../shared/types.hpp"
#include "../at-state/index.hpp"
#include "../bonus-state/index.hpp"

namespace slotv2::entry_gate {

enum class Kind : uint8_t {
    None = 0,
    Bonus = 1,
    AT = 2
};

struct State {
    bool active{false};
    bool armed_this_game{false};
    Kind kind{Kind::None};
    bonus_state::Kind bonus_kind{bonus_state::Kind::Regular};
    bool bonus_return_to_at{false};
    at_state::Tier at_tier{at_state::Tier::Lower};
    uint32_t stock_to_add{0};
};

void clear(State& state);
void queueBonus(
    State& state,
    bonus_state::Kind kind,
    bool return_to_at
);
void queueAT(
    State& state,
    at_state::Tier tier,
    uint32_t stock_to_add = 0u
);

// Waiting hit is already internally fixed. Each following game gets
// a fresh 1/2 chance to reactivate the start-symbol attempt.
bool beginGame(State& state, Rng& rng);
RoleFlag roleForArmed(const State& state);
bool matches(
    const State& state,
    Symbol left,
    Symbol middle,
    Symbol right
);

} // namespace slotv2::entry_gate
