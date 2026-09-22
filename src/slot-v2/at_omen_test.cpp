#include <iostream>
#include "at-omen/index.hpp"

int main() {
    bool ok = true;

    {
        slotv2::at_state::State at{};
        slotv2::at_state::start(
            at,
            slotv2::at_state::Tier::Lower
        );
        slotv2::entry_gate::State entry{};
        slotv2::at_omen::State omen{};
        slotv2::at_omen::start(omen, false);

        const auto game = slotv2::at_omen::beginGame(
            at,
            omen
        );

        ok = ok && game.active;
        ok = ok && game.ended;
        ok = ok && at.games_left == 99;

        const auto out = slotv2::at_omen::finalize(
            at,
            entry,
            omen,
            game
        );

        ok = ok && out
            == slotv2::at_omen::FinalizeOutcome::BonusEntryQueued;
        ok = ok && entry.active;
        ok = ok && entry.bonus_kind
            == slotv2::bonus_state::Kind::Regular;
        ok = ok && entry.bonus_return_to_at;
    }

    {
        slotv2::at_state::State at{};
        slotv2::at_state::start(
            at,
            slotv2::at_state::Tier::Upper
        );
        at.games_left = 0;
        slotv2::entry_gate::State entry{};
        slotv2::at_omen::State omen{};
        slotv2::at_omen::start(omen, true);

        const auto game = slotv2::at_omen::beginGame(
            at,
            omen
        );

        ok = ok && game.ended;
        ok = ok && game.at_window_empty;
        ok = ok && at.games_left == 0;

        const auto out = slotv2::at_omen::finalize(
            at,
            entry,
            omen,
            game
        );

        ok = ok && out
            == slotv2::at_omen::FinalizeOutcome::EpisodeEntryQueued;
        ok = ok && entry.active;
        ok = ok && entry.bonus_kind
            == slotv2::bonus_state::Kind::Episode;
    }

    if (!ok) {
        std::cerr << "slot_v2_at_omen_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_at_omen_test: OK\n";
    return 0;
}
