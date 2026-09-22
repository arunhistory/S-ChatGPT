#pragma once
#include "../shared/rng.hpp"
#include "../machine-state/index.hpp"
#include "../normal-mode/index.hpp"
#include "../normal-route/index.hpp"

namespace slotv2::normal_cycle_reset {

void apply(
    Rng& rng,
    machine_state::State& machine,
    normal_mode::Mode& mode,
    normal_route::State& route
);

} // namespace slotv2::normal_cycle_reset
