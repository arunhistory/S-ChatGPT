#include <iostream>
#include "core/runtime.hpp"

namespace {

void stopAll(slotv2::runtime::State& state) {
    (void)slotv2::runtime::stop(state, 0u, 0u);
    (void)slotv2::runtime::stop(state, 1u, 0u);
    (void)slotv2::runtime::stop(state, 2u, 0u);
}

}

int main() {
    bool ok = true;

    // Internal AT hit -> one omen game -> RED bonus entry wait.
    {
        slotv2::runtime::State state{};
        slotv2::runtime::reset(state, 0x0A0AULL);

        slotv2::at_state::start(
            state.machine.at,
            slotv2::at_state::Tier::Middle
        );
        state.machine.area = slotv2::machine_state::Area::AT;

        // Reproduce the state immediately after an internally won AT hit game.
        slotv2::at_omen::start(
            state.machine.at_omen,
            false
        );

        const int before = state.machine.at.games_left;
        const uint32_t lever = slotv2::runtime::lever(state);

        ok = ok && ((lever >> 24) & 0xffu)
            == static_cast<uint32_t>(slotv2::CommandStatus::Ok);
        ok = ok && state.at_omen_game.active;
        ok = ok && state.at_omen_game.ended;
        ok = ok && state.machine.at.games_left == before - 1;
        ok = ok && !state.at_cycle.active;
        ok = ok && !state.machine.entry_gate.active;

        stopAll(state);

        ok = ok && state.at_omen_finalize
            == slotv2::at_omen::FinalizeOutcome::BonusEntryQueued;
        ok = ok && !state.machine.at_omen.active;
        ok = ok && state.machine.entry_gate.active;
        ok = ok && state.machine.entry_gate.kind
            == slotv2::entry_gate::Kind::Bonus;
        ok = ok && state.machine.entry_gate.bonus_kind
            == slotv2::bonus_state::Kind::Regular;
        ok = ok && state.machine.entry_gate.bonus_return_to_at;
    }

    // If the AT window is already empty, omen/entry wait must still outrank AT end.
    {
        slotv2::runtime::State state{};
        slotv2::runtime::reset(state, 0x0E0EULL);

        slotv2::at_state::start(
            state.machine.at,
            slotv2::at_state::Tier::Upper
        );
        state.machine.at.games_left = 0;
        state.machine.area = slotv2::machine_state::Area::AT;

        slotv2::pending_event::add(
            state.pending,
            slotv2::pending_event::ATWindowEmpty
        );
        slotv2::at_omen::start(
            state.machine.at_omen,
            true
        );

        const uint32_t lever = slotv2::runtime::lever(state);

        ok = ok && ((lever >> 24) & 0xffu)
            == static_cast<uint32_t>(slotv2::CommandStatus::Ok);
        ok = ok && state.machine.area == slotv2::machine_state::Area::AT;
        ok = ok && state.machine.at.active;
        ok = ok && state.at_omen_game.active;
        ok = ok && state.at_omen_game.at_window_empty;

        stopAll(state);

        ok = ok && state.at_omen_finalize
            == slotv2::at_omen::FinalizeOutcome::EpisodeEntryQueued;
        ok = ok && state.machine.entry_gate.active;
        ok = ok && state.machine.entry_gate.bonus_kind
            == slotv2::bonus_state::Kind::Episode;
        ok = ok && slotv2::pending_event::has(
            state.pending,
            slotv2::pending_event::ATWindowEmpty
        );

        // The next lever is the RED entry-wait game, not AT termination.
        const uint32_t waitLever = slotv2::runtime::lever(state);
        ok = ok && ((waitLever >> 24) & 0xffu)
            == static_cast<uint32_t>(slotv2::CommandStatus::Ok);
        ok = ok && state.machine.area == slotv2::machine_state::Area::AT;
        ok = ok && state.machine.at.active;
    }

    if (!ok) {
        std::cerr << "slot_v2_runtime_at_omen_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_runtime_at_omen_test: OK\n";
    return 0;
}
