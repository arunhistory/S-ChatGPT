#include <iostream>
#include "at/lower_fall_challenge.hpp"

int main() {
    bool ok = true;
    bool saw_success = false;
    bool saw_failure = false;
    bool saw_non_trigger_wait = false;
    bool saw_judge_bell = false;

    for (uint64_t seed = 1u;
         seed < 5000u && (!saw_success || !saw_failure || !saw_non_trigger_wait || !saw_judge_bell);
         ++seed) {
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

        const bool fixed_success = state.success_fixed;

        ok = ok && slotv2::lower_fall_challenge::beginWaiting(state);
        ok = ok && state.phase
            == slotv2::lower_fall_challenge::Phase::Waiting;

        bool ready = false;
        for (int game = 0; game < 64 && !ready; ++game) {
            const auto wait =
                slotv2::lower_fall_challenge::beginWaitGame(
                    rng,
                    state
                );

            ok = ok && wait.active;

            if (wait.judge_bell) {
                saw_judge_bell = true;
            } else {
                saw_non_trigger_wait = true;
            }

            ready = slotv2::lower_fall_challenge::finalizeWaitGame(
                state,
                wait
            );

            if (!wait.judge_bell) {
                ok = ok && !ready;
                ok = ok && state.phase
                    == slotv2::lower_fall_challenge::Phase::Waiting;
            } else {
                ok = ok && ready;
                ok = ok && slotv2::lower_fall_challenge::buttonReady(state);
            }
        }

        ok = ok && ready;

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

        if (fixed_success) {
            saw_success = true;
            ok = ok && outcome
                == slotv2::lower_fall_challenge::PushOutcome::Continued;
            ok = ok && at.games_left == 50;
        } else {
            saw_failure = true;
            ok = ok && outcome
                == slotv2::lower_fall_challenge::PushOutcome::Failed;
            ok = ok && at.games_left == 0;
        }

        ok = ok && state.phase
            == slotv2::lower_fall_challenge::Phase::Inactive;
    }

    ok = ok
        && saw_success
        && saw_failure
        && saw_non_trigger_wait
        && saw_judge_bell;

    if (!ok) {
        std::cerr << "slot_v2_lower_fall_challenge_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_lower_fall_challenge_test: OK\n";
    return 0;
}
