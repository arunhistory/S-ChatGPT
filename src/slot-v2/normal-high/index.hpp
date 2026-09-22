#pragma once
#include <stdint.h>
#include "../shared/rng.hpp"
#include "../shared/types.hpp"
#include "../normal-shortening/index.hpp"

namespace slotv2::normal_high {

struct State {
    bool active{false};
    uint16_t games{0};
};

enum class Reward : uint8_t {
    None = 0,
    CZ = 1,
    Bonus = 2
};

struct Result {
    bool entered{false};
    bool exited{false};
    uint16_t shorten_games{0};
    Reward reward{Reward::None};
};

uint16_t entryThresholdPerThousand(RoleFlag role);
uint16_t hitThresholdPerThousand(RoleFlag role);

Result play(
    Rng& rng,
    State& state,
    RoleFlag role,
    uint32_t actual_games
);

void reset(State& state);

} // namespace slotv2::normal_high
