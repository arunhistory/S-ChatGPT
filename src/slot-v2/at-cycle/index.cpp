#include "index.hpp"
#include "../at-lottery/index.hpp"

namespace slotv2::at_cycle {

Result beginGame(Rng& rng, const machine_state::State& machine) {
    if (machine.area != machine_state::Area::AT || !machine.at.active) {
        return {};
    }

    const auto draw = at_lottery::drawBase(rng);
    return {
        true,
        at_event::fromDraw(draw)
    };
}

} // namespace slotv2::at_cycle
