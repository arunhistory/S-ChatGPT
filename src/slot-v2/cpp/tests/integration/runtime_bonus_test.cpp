#include <iostream>
#include "core/runtime.hpp"
#include "bonus/bonus_transition.hpp"

int main() {
    bool ok = true;
    auto check = [&](bool cond, const char* label) {
        if (!cond) {
            std::cerr << "FAIL: " << label << "\n";
            ok = false;
        }
    };

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

        check(r.outcome == slotv2::bonus_cycle::Outcome::Completed,
            "episode reaches Completed");
        check(state.machine.area == slotv2::machine_state::Area::AT,
            "episode returns to AT");
        check(!state.machine.bonus.active,
            "episode bonus inactive after completion");
        check(!state.machine.bonus_return_valid,
            "bonus return latch cleared");
        check(state.bonus_transition.outcome
            == slotv2::bonus_transition::Outcome::Returned,
            "transition Returned only on completion");
        check(!slotv2::pending_event::has(
            state.pending,
            slotv2::pending_event::BonusComplete
        ), "BonusComplete pending consumed");

        // Make the return target a real active AT before starting the next game.
        // This mirrors the reported AT -> BONUS -> AT path.
        slotv2::at_state::start(
            state.machine.at,
            slotv2::at_state::Tier::Middle
        );
        state.machine.area = slotv2::machine_state::Area::AT;

        // BONUS completion is a one-game result. On the first returned AT
        // lever it must be cleared, otherwise UI/end-flow repeats every game.
        const auto next = slotv2::runtime::lever(state);
        check(((next >> 24) & 0xffu) == 0u,
            "first AT lever accepted after BONUS");
        check(state.machine.area == slotv2::machine_state::Area::AT,
            "still in AT after returned lever");
        check(state.bonus_cycle.outcome
            == slotv2::bonus_cycle::Outcome::None,
            "bonus cycle result cleared on next AT lever");
        check(state.bonus_transition.outcome
            == slotv2::bonus_transition::Outcome::None,
            "bonus transition result cleared on next AT lever");
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

        check(a.outcome == slotv2::bonus_cycle::Outcome::None,
            "regular 49/50 is not completed");
        check(state.machine.bonus.active,
            "regular remains active before target");
        check(state.machine.bonus.medals_left == 1,
            "regular has 1 medal left");
        check(state.bonus_transition.outcome
            == slotv2::bonus_transition::Outcome::None,
            "no finalize transition before target");

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
