#pragma once
#include "shared/rng.hpp"
#include "core/machine_state.hpp"
#include "normal/normal_mode.hpp"
#include "normal/normal_route.hpp"

namespace slotv2::normal_cycle_reset {

void apply(
    Rng& rng,
    machine_state::State& machine,
    normal_mode::Mode& mode,
    normal_route::State& route
);

} // namespace slotv2::normal_cycle_reset
