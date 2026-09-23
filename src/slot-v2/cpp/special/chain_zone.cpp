#include "special/chain_zone.hpp"

namespace slotv2::chain_zone {
namespace {
bool eligible(RoleFlag role) {
    return role == RoleFlag::Bell9 || role == RoleFlag::Bell15
        || role == RoleFlag::Replay;
}
}

void start(State& state) {
    state = {};
    state.active = true;
    state.games_left = kGamesPerSet;
}

bool continuationFromRoll(RoleFlag role, uint8_t roll_0_to_8) {
    // 1/15 + 1/80 + 1/30 = 0.1125 eligible roles.
    // 8/9 acceptance = approximately 0.1 per zone game.
    return eligible(role) && (roll_0_to_8 % 9u) < 8u;
}

Step playOne(State& state, Rng& rng, RoleFlag role) {
    if (!state.active || state.games_left == 0u) return {};
    Step result{};
    result.played = true;

    // A continuation hit is latched; the current five-game set always
    // finishes before the next five-game set starts.
    if (!state.signalled_this_set
        && eligible(role)
        && continuationFromRoll(
            role, static_cast<uint8_t>(rng.uniformBelow(9u))
        )) {
        state.signalled_this_set = true;
        result.signal_found = true;
    }

    --state.games_left;
    if (state.games_left > 0u) return result;

    if (state.signalled_this_set) {
        ++state.continuations;
        state.games_left = kGamesPerSet;
        state.signalled_this_set = false;
        result.continued = true;
        return result;
    }

    state.active = false;
    state.bonus_remaining = state.continuations / 2u;
    result.ended = true;
    result.earned_bonuses = state.bonus_remaining;
    return result;
}

uint16_t episodeThresholdPerThousand(uint8_t batch_size) {
    switch (batch_size) {
        case 2u: return 50u;
        case 3u: return 75u;
        case 4u: return 100u;
        default: return 0u;
    }
}

Release releaseNext(State& state, Rng& rng) {
    if (state.active || state.bonus_remaining == 0u) return {};

    if (state.batch_remaining == 0u) {
        // No earned hit is dropped even if a very rare chain exceeds eight.
        // Split into sequential batches of up to four bonuses.
        state.batch_size = static_cast<uint8_t>(
            state.bonus_remaining < 4u ? state.bonus_remaining : 4u
        );
        state.batch_remaining = state.batch_size;
        state.episode_position = 0u;
        const auto threshold =
            episodeThresholdPerThousand(state.batch_size);
        if (threshold > 0u && rng.uniformBelow(1000u) < threshold) {
            state.episode_position = static_cast<uint8_t>(
                1u + rng.uniformBelow(state.batch_size)
            );
        }
    }

    const uint8_t index = static_cast<uint8_t>(
        state.batch_size - state.batch_remaining + 1u
    );
    const Release result{
        true,
        index == state.episode_position,
        state.batch_size >= 2u,
        state.batch_size
    };
    --state.batch_remaining;
    --state.bonus_remaining;
    return result;
}

} // namespace slotv2::chain_zone
