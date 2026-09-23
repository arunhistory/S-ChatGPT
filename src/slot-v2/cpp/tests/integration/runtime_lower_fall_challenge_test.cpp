#include <iostream>
#include "core/runtime.hpp"

namespace {

void stopAll(slotv2::runtime::State& state) {
    (void)slotv2::runtime::stop(state, 0u, 0u);
    (void)slotv2::runtime::stop(state, 1u, 0u);
    (void)slotv2::runtime::stop(state, 2u, 0u);
}

bool runOne(uint64_t seed, bool want_success) {
    slotv2::runtime::State state{};
    slotv2::runtime::reset(state, seed);

    slotv2::at_state::start(
        state.machine.at,
        slotv2::at_state::Tier::Lower
    );
    state.machine.area = slotv2::machine_state::Area::AT;

    const uint32_t lever = slotv2::runtime::lever(state);
    if (((lever >> 24) & 0xffu)
        != static_cast<uint32_t>(slotv2::CommandStatus::Ok)) {
        return false;
    }

    if (state.at_resolution.status
            != slotv2::at_resolution::Status::Single
        || state.at_resolution.event
            != slotv2::at_resolution::Event::Fall) {
        return false;
    }

    const auto& armed = state.machine.lower_fall_challenge;
    if (armed.phase != slotv2::lower_fall_challenge::Phase::Armed) {
        return false;
    }
    if (armed.saved_games != 100) return false;
    if (state.machine.at.games_left != 99) return false;
    if (armed.success_fixed != want_success) return false;

    // The Fall game itself finishes normally.
    stopAll(state);

    // First wait game. Resolving ATFall changes Armed -> Waiting before this spin.
    const uint32_t wait1 = slotv2::runtime::lever(state);
    if (((wait1 >> 24) & 0xffu)
        != static_cast<uint32_t>(slotv2::CommandStatus::Ok)) {
        return false;
    }
    if (!state.lower_fall_wait_game.active
        || state.lower_fall_wait_game.games_before != 2u) {
        return false;
    }
    if (state.at_cycle.active) return false;
    if (state.machine.at.games_left != 99) return false;

    stopAll(state);
    if (state.machine.lower_fall_challenge.wait_games_left != 1u) {
        return false;
    }

    // Second wait game.
    const uint32_t wait2 = slotv2::runtime::lever(state);
    if (((wait2 >> 24) & 0xffu)
        != static_cast<uint32_t>(slotv2::CommandStatus::Ok)) {
        return false;
    }
    if (!state.lower_fall_wait_game.active
        || state.lower_fall_wait_game.games_before != 1u) {
        return false;
    }

    stopAll(state);

    if (!slotv2::lower_fall_challenge::buttonReady(
            state.machine.lower_fall_challenge
        )) {
        return false;
    }

    // Lever is locked until the one-shot button is pressed.
    const uint32_t blocked = slotv2::runtime::lever(state);
    if (((blocked >> 24) & 0xffu)
        != static_cast<uint32_t>(slotv2::CommandStatus::RejectedPhase)) {
        return false;
    }

    const auto pushed =
        static_cast<slotv2::lower_fall_challenge::PushOutcome>(
            slotv2::runtime::pushLowerFallChallenge(state)
        );

    if (want_success) {
        if (pushed
            != slotv2::lower_fall_challenge::PushOutcome::Continued) {
            return false;
        }

        // Exact pre-Fall value is restored: 100G, not 99G.
        if (state.machine.at.games_left != 100) return false;
        if (state.machine.area != slotv2::machine_state::Area::AT) return false;
        if (!state.machine.at.active) return false;
        if (state.machine.at.tier != slotv2::at_state::Tier::Lower) return false;

        return true;
    }

    if (pushed
        != slotv2::lower_fall_challenge::PushOutcome::Failed) {
        return false;
    }

    if (state.machine.at.games_left != 0) return false;
    if (!slotv2::pending_event::has(
            state.pending,
            slotv2::pending_event::ATWindowEmpty
        )) {
        return false;
    }

    // Existing complete-end flow owns what happens next.
    // With no stock, Lower proceeds to the existing five-game revival.
    const uint32_t after_fail = slotv2::runtime::lever(state);
    if (((after_fail >> 24) & 0xffu)
        != static_cast<uint32_t>(slotv2::CommandStatus::Ok)) {
        return false;
    }

    return state.machine.area == slotv2::machine_state::Area::Revival
        && state.machine.revival.active
        && state.machine.revival.games_left == 4u;
}

}

int main() {
    bool success_ok = false;
    bool failure_ok = false;

    for (uint64_t seed = 1u;
         seed < 200000u && (!success_ok || !failure_ok);
         ++seed) {
        if (!success_ok) {
            success_ok = runOne(seed, true);
        }
        if (!failure_ok) {
            failure_ok = runOne(seed, false);
        }
    }

    if (!success_ok || !failure_ok) {
        std::cerr << "slot_v2_runtime_lower_fall_challenge_test: FAILED"
                  << " success=" << success_ok
                  << " failure=" << failure_ok
                  << "\n";
        return 1;
    }

    std::cout << "slot_v2_runtime_lower_fall_challenge_test: OK\n";
    return 0;
}
