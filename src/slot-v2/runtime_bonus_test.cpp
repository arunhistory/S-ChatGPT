#include <iostream>
#include "runtime/index.hpp"

int main() {
    bool ok = true;

    {
        slotv2::runtime::State state{};
        slotv2::runtime::reset(state, 0xB050ULL);

        state.machine.area = slotv2::machine_state::Area::Bonus;
        slotv2::bonus_state::start(
            state.machine.bonus,
            slotv2::bonus_state::Kind::Regular
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

        ok = ok && !state.machine.bonus.active;
        ok = ok && state.machine.bonus.medals_left == 0;
        ok = ok && (
            b.outcome == slotv2::bonus_cycle::Outcome::Completed
            || b.outcome == slotv2::bonus_cycle::Outcome::EpisodeUpgradePending
        );

        const uint32_t pending = slotv2::runtime::pendingEvents(state);
        const bool completion =
            (pending & slotv2::pending_event::BonusComplete) != 0u;
        const bool upgrade =
            (pending & slotv2::pending_event::BonusEpisodeUpgrade) != 0u;

        ok = ok && (completion != upgrade);
    }

    {
        slotv2::runtime::State state{};
        slotv2::runtime::reset(state, 0xB080ULL);

        state.machine.area = slotv2::machine_state::Area::Bonus;
        slotv2::bonus_state::start(
            state.machine.bonus,
            slotv2::bonus_state::Kind::Episode
        );

        const auto r = slotv2::runtime::applyBonusNetGain(
            state,
            80
        );

        ok = ok
            && r.outcome == slotv2::bonus_cycle::Outcome::Completed;

        ok = ok && (
            slotv2::runtime::pendingEvents(state)
            & slotv2::pending_event::BonusComplete
        ) != 0u;
    }

    if (!ok) {
        std::cerr << "slot_v2_runtime_bonus_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_runtime_bonus_test: OK\n";
    return 0;
}
