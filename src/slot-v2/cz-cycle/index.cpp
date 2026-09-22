#include "index.hpp"
#include "../cz-lottery/index.hpp"

namespace slotv2::cz_cycle {

Result playOne(Rng& rng, cz_state::State& state) {
    if (!state.active || state.games_left == 0u) return {};

    const bool hit = cz_lottery::drawBase(rng);
    const bool still_active = cz_state::consumeGame(state);

    return {
        true,
        hit,
        state.games_left,
        !still_active && state.games_left == 0u
    };
}

} // namespace slotv2::cz_cycle
