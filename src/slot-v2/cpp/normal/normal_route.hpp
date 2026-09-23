#pragma once
#include <stdint.h>
#include "shared/rng.hpp"
#include "normal/normal_mode.hpp"
#include "special/special_ceiling.hpp"

namespace slotv2::normal_route {

struct State {
    uint8_t pattern{0};
    uint16_t ceiling{0};
    bool special_window_checked{false};
    bool ceiling_consumed{false};
};

uint16_t ceilingFor(
    normal_mode::Mode mode,
    uint8_t pattern
);

void rerollBase(
    Rng& rng,
    normal_mode::Mode& mode,
    State& state
);

// SuperHeaven: once actual normal games reach 20, check exactly once.
// 10% enters Special and chooses 777(95%) / 1500(5%).
bool checkSpecialWindow(
    Rng& rng,
    normal_mode::Mode& mode,
    State& state,
    uint32_t actual_games
);

bool reached(
    const State& state,
    uint32_t display_games
);

void consumeCeiling(State& state);

} // namespace slotv2::normal_route
