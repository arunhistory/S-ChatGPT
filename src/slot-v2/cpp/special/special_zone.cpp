#include "index.hpp"

namespace slotv2::special_zone {

void start(State& state) {
    state.active = true;
    state.games_left = kGames;
    state.pending_add_games = 0u;
}

uint16_t addGamesFromRoll(uint16_t roll_0_to_99) {
    const uint16_t roll = static_cast<uint16_t>(roll_0_to_99 % 100u);

    if (roll < 30u) return 20u;
    if (roll < 55u) return 30u;
    if (roll < 75u) return 40u;
    if (roll < 90u) return 50u;
    if (roll < 98u) return 100u;
    return 200u;
}

HitResult playOne(State& state, Rng& rng) {
    if (!state.active || state.games_left == 0u) return HitResult::None;

    state.pending_add_games = 0u;

    --state.games_left;
    if (state.games_left == 0u) state.active = false;

    // Each game: 1/2 hit. On hit: 95% add-games / 5% regular bonus.
    if (!rng.oneIn(2u)) return HitResult::None;

    if (rng.oneIn(20u)) {
        // Bonus ends the regular special zone immediately.
        state.active = false;
        state.games_left = 0u;
        return HitResult::Bonus;
    }

    state.pending_add_games = addGamesFromRoll(
        static_cast<uint16_t>(rng.uniformBelow(100u))
    );
    return HitResult::AddGames;
}

} // namespace slotv2::special_zone
