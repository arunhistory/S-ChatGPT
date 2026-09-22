#include "index.hpp"
#include "../at-lottery/index.hpp"

namespace slotv2::at_cycle {

Result beginGame(Rng& rng, machine_state::State& machine) {
    if (machine.area != machine_state::Area::AT || !machine.at.active) {
        return {};
    }

    const int32_t before = machine.at.games_left;
    if (before <= 0) {
        return {
            true,
            {},
            before,
            before,
            true
        };
    }

    const auto draw = at_lottery::draw(rng, machine.at);
    (void)at_state::consumeGame(machine.at);

    const int32_t after = machine.at.games_left;

    return {
        true,
        at_event::fromDraw(draw),
        before,
        after,
        after <= 0
    };
}

} // namespace slotv2::at_cycle
