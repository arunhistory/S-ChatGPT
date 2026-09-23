#include <iostream>
#include "at/lower_fall_challenge.hpp"

int main() {
    bool ok = true;

    bool saw_success = false;
    bool saw_failure = false;

    for (uint64_t seed = 1u; seed < 1000u && (!saw_success || !saw_failure); ++seed) {
        slotv2::Rng rng(seed);
        slotv2::lower_fall_challenge::State state{};
        slotv2::lower_fall_challenge::arm(
            rng,
            state,
            50
        );

        ok = ok && state.phase
            == slotv2::lower_fall_challenge::Phase::Armed;
        ok = ok && state.saved_games == 50;
        ok = ok && state.wait_games_left == 2u;

        ok = ok && slotv2::lower_fall_challenge::beginWaiting(state);

        auto first = slotv2::lower_fall_challenge::beginWaitGame(state);
        ok = ok && first.active && first.games_before == 2u;
        ok = ok && !slotv2::lower_fall_challenge::finalizeWaitGame(
            state,
            first
        );
        ok = ok && state.wait_games_left == 1u;

        auto second = slotv2::lower_fall_challenge::beginWaitGame(state);
        ok = ok && second.active && second.games_before == 1u;
        ok = ok && slotv2::lower_fall_challenge::finalizeWaitGame(
            state,
            second
        );
        ok = ok && slotv2::lower_fall_challenge::buttonReady(state);

        slotv2::at_state::State at{};
        slotv2::at_state::start(
            at,
            slotv2::at_state::Tier::Lower
        );
        at.games_left = 49;

        const auto outcome = slotv2::lower_fall_challenge::push(
            state,
            at
        );

        if (outcome
            == slotv2::lower_fall_challenge::PushOutcome::Continued) {
            saw_success = true;
            ok = ok && at.games_left == 50;
        } else if (outcome
            == slotv2::lower_fall_challenge::PushOutcome::Failed) {
            saw_failure = true;
            ok = ok && at.games_left == 0;
        } else {
            ok = false;
        }

        ok = ok && state.phase
            == slotv2::lower_fall_challenge::Phase::Inactive;
    }

    ok = ok && saw_success && saw_failure;

    if (!ok) {
        std::cerr << "slot_v2_lower_fall_challenge_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_lower_fall_challenge_test: OK\n";
    return 0;
}
