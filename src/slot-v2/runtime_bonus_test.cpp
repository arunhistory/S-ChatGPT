#include <iostream>
#include "runtime/index.hpp"
#include "bonus-transition/index.hpp"

int main() {
    bool ok = true;

    {
        slotv2::runtime::State state{};
        slotv2::runtime::reset(state, 0xB080ULL);

        slotv2::bonus_transition::start(
            state.machine,
            slotv2::bonus_state::Kind::Episode,
            slotv2::machine_state::Area::AT
        );

        const auto r = slotv2::runtime::applyBonusNetGain(
            state,
            80
        );

        ok = ok && r.outcome == slotv2::bonus_cycle::Outcome::Completed;
        ok = ok && state.machine.area == slotv2::machine_state::Area::AT;
        ok = ok && !state.machine.bonus.active;
        ok = ok && !state.machine.bonus_return_valid;
        ok = ok && state.bonus_transition.outcome
            == slotv2::bonus_transition::Outcome::Returned;
        ok = ok && !slotv2::pending_event::has(
            state.pending,
            slotv2::pending_event::BonusComplete
        );
    }

    // Regular BONUS may either end normally or roll the specified 1% Episode upgrade.
    // Both paths must be internally finalized without leaving a stale pending bit.
    {
        slotv2::runtime::State state{};
        slotv2::runtime::reset(state, 0xB050ULL);

        slotv2::bonus_transition::start(
            state.machine,
            slotv2::bonus_state::Kind::Regular,
            slotv2::machine_state::Area::Normal
        );

        const auto a = slotv2::runtime::applyBonusNetGain(
            state,
            49
        );

        ok = ok && a.outcome == slotv2::bonus_cycle::Outcome::None;
        ok = ok && state.machine.bonus.active;
        ok = ok && state.machine.bonus.medals_left == 1;

        const auto b = slotv2::runtime::applyBonusNetGain(
            state,
            1
        );

        if (b.outcome == slotv2::bonus_cycle::Outcome::Completed) {
            ok = ok && state.machine.area == slotv2::machine_state::Area::Normal;
            ok = ok && !state.machine.bonus.active;
            ok = ok && state.bonus_transition.outcome
                == slotv2::bonus_transition::Outcome::Returned;
        } else {
            ok = ok && b.outcome
                == slotv2::bonus_cycle::Outcome::EpisodeUpgradePending;
            ok = ok && state.machine.area == slotv2::machine_state::Area::Bonus;
            ok = ok && state.machine.bonus.active;
            ok = ok && state.machine.bonus.kind
                == slotv2::bonus_state::Kind::Episode;
            ok = ok && state.machine.bonus.medals_left == 80;
            ok = ok && state.bonus_transition.outcome
                == slotv2::bonus_transition::Outcome::EpisodeStarted;
        }

        ok = ok && !slotv2::pending_event::has(
            state.pending,
            slotv2::pending_event::BonusComplete
        );
        ok = ok && !slotv2::pending_event::has(
            state.pending,
            slotv2::pending_event::BonusEpisodeUpgrade
        );
    }

    if (!ok) {
        std::cerr << "slot_v2_runtime_bonus_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_runtime_bonus_test: OK\n";
    return 0;
}
