#include "index.hpp"
#include "../cz-lottery/index.hpp"

namespace slotv2::cz_cycle {

Result playOne(Rng& rng, cz_state::State& state) {
    if (!state.active || state.games_left == 0u) return {};

    const bool hit = cz_lottery::drawBase(rng);

    if (hit) {
        cz_state::resolve(state);
        return {
            true,
            true,
            0,
            true
        };
    }

    const bool still_active = cz_state::consumeGame(state);

    return {
        true,
        false,
        state.games_left,
        !still_active && state.games_left == 0u
    };
}

} // namespace slotv2::cz_cycle
