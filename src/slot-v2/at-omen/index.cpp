#include "index.hpp"

namespace slotv2::at_omen {

void start(State& state, bool episode) {
    state.active = true;
    state.episode = episode;
    state.games_left = 1u;
}

void clear(State& state) {
    state = {};
}

Game beginGame(
    at_state::State& at,
    State& omen
) {
    if (!omen.active || omen.games_left == 0u) return {};

    Game out{};
    out.active = true;
    out.episode = omen.episode;
    out.omen_before = omen.games_left;

    if (at.active && at.games_left > 0) {
        (void)at_state::consumeGame(at);
    }

    --omen.games_left;
    out.omen_after = omen.games_left;
    out.ended = omen.games_left == 0u;
    out.at_window_empty = at.active && at.games_left <= 0;

    return out;
}

FinalizeOutcome finalize(
    const at_state::State& at,
    entry_gate::State& entry,
    State& omen,
    const Game& game
) {
    if (!game.active || !game.ended || !at.active) {
        return FinalizeOutcome::None;
    }

    entry_gate::queueBonus(
        entry,
        game.episode
            ? bonus_state::Kind::Episode
            : bonus_state::Kind::Regular,
        true
    );

    const auto outcome = game.episode
        ? FinalizeOutcome::EpisodeEntryQueued
        : FinalizeOutcome::BonusEntryQueued;

    clear(omen);
    return outcome;
}

} // namespace slotv2::at_omen
