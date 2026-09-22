#include "index.hpp"

namespace slotv2::normal_cycle_reset {

void apply(
    Rng& rng,
    machine_state::State& machine,
    normal_mode::Mode& mode,
    normal_route::State& route
) {
    normal_state::reset(machine.normal);
    machine.normal_progress.bell9_streak = 0u;
    normal_route::rerollBase(
        rng,
        mode,
        route
    );
}

} // namespace slotv2::normal_cycle_reset
