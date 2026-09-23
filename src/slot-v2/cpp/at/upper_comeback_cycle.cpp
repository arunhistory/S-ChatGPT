#include "at/upper_comeback_cycle.hpp"

namespace slotv2::upper_comeback_cycle {

Result playOne(
    Rng& rng,
    upper_comeback::State& state
) {
    if (!state.active || state.games_left == 0u) return {};

    Result out{};
    out.active_before = true;
    out.games_before = state.games_left;

    (void)upper_comeback::consumeGame(state);

    out.games_after = state.games_left;
    out.ended = !state.active && state.games_left == 0u;

    if (out.ended) {
        out.hit = upper_comeback::judge(rng);
    }

    return out;
}

} // namespace slotv2::upper_comeback_cycle
