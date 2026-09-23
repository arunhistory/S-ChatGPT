#include "special/upper_special.hpp"

namespace slotv2::upper_special {

void start(State& state) {
    state.active = true;
    state.rounds = 0u;
    state.total_added_games = 0u;
}

uint16_t addGamesFromRoll(uint16_t roll_0_to_99) {
    const uint16_t roll = static_cast<uint16_t>(roll_0_to_99 % 100u);

    if (roll < 29u) return 5u;   // 29%
    if (roll < 59u) return 10u;  // 30%
    if (roll < 78u) return 20u;  // 19%
    if (roll < 89u) return 30u;  // 11%
    if (roll < 97u) return 50u;  // 8%
    return 100u;                 // 3%
}

Step playOne(State& state, Rng& rng) {
    if (!state.active) return {};

    Step out{};
    out.active_before = true;
    out.round = ++state.rounds;

    // No guarantee. 10% ends immediately; 90% succeeds and adds games.
    if (rng.uniformBelow(100u) >= 90u) {
        state.active = false;
        out.ended = true;
        return out;
    }

    out.success = true;
    out.added_games = addGamesFromRoll(
        static_cast<uint16_t>(rng.uniformBelow(100u))
    );
    state.total_added_games += out.added_games;
    return out;
}

} // namespace slotv2::upper_special
