#include <iostream>
#include "at-omen/index.hpp"

int main() {
    bool ok = true;

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);
        slotv2::at_state::start(
            machine.at,
            slotv2::at_state::Tier::Lower
        );
        machine.area = slotv2::machine_state::Area::AT;

        slotv2::at_omen::State omen{};
        slotv2::at_omen::start(omen, false);

        const auto game = slotv2::at_omen::beginGame(
            machine.at,
            omen
        );

        ok = ok && game.active;
        ok = ok && game.ended;
        ok = ok && machine.at.games_left == 99;

        const auto out = slotv2::at_omen::finalize(
            machine,
            omen,
            game
        );

        ok = ok && out
            == slotv2::at_omen::FinalizeOutcome::BonusEntryQueued;
        ok = ok && machine.entry_gate.active;
        ok = ok && machine.entry_gate.bonus_kind
            == slotv2::bonus_state::Kind::Regular;
        ok = ok && machine.entry_gate.bonus_return_to_at;
    }

    // Final AT game already at 0: omen still gets its presentation game,
    // then the queued BONUS owns priority before AT end processing.
    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);
        slotv2::at_state::start(
            machine.at,
            slotv2::at_state::Tier::Upper
        );
        machine.at.games_left = 0;
        machine.area = slotv2::machine_state::Area::AT;

        slotv2::at_omen::State omen{};
        slotv2::at_omen::start(omen, true);

        const auto game = slotv2::at_omen::beginGame(
            machine.at,
            omen
        );

        ok = ok && game.ended;
        ok = ok && game.at_window_empty;
        ok = ok && machine.at.games_left == 0;

        const auto out = slotv2::at_omen::finalize(
            machine,
            omen,
            game
        );

        ok = ok && out
            == slotv2::at_omen::FinalizeOutcome::EpisodeEntryQueued;
        ok = ok && machine.entry_gate.active;
        ok = ok && machine.entry_gate.bonus_kind
            == slotv2::bonus_state::Kind::Episode;
    }

    if (!ok) {
        std::cerr << "slot_v2_at_omen_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_at_omen_test: OK\n";
    return 0;
}
