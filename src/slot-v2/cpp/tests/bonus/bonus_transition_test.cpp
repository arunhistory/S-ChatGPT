#include <iostream>
#include "bonus/bonus_transition.hpp"

int main() {
    bool ok = true;

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);
        slotv2::pending_event::State pending{};

        slotv2::bonus_transition::start(
            machine,
            slotv2::bonus_state::Kind::Regular,
            slotv2::machine_state::Area::AT
        );

        ok = ok && machine.area == slotv2::machine_state::Area::Bonus;
        ok = ok && machine.bonus_return_valid;
        ok = ok && machine.bonus_return_area == slotv2::machine_state::Area::AT;

        slotv2::pending_event::add(
            pending,
            slotv2::pending_event::BonusComplete
        );

        slotv2::bonus_cycle::Result completed{};
        completed.outcome = slotv2::bonus_cycle::Outcome::Completed;

        const auto r = slotv2::bonus_transition::finalize(
            machine,
            pending,
            completed
        );

        ok = ok && r.outcome == slotv2::bonus_transition::Outcome::Returned;
        ok = ok && machine.area == slotv2::machine_state::Area::AT;
        ok = ok && !machine.bonus_return_valid;
        ok = ok && !slotv2::pending_event::has(
            pending,
            slotv2::pending_event::BonusComplete
        );
    }

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);
        slotv2::pending_event::State pending{};

        slotv2::bonus_transition::start(
            machine,
            slotv2::bonus_state::Kind::Regular,
            slotv2::machine_state::Area::Normal
        );

        slotv2::pending_event::add(
            pending,
            slotv2::pending_event::BonusEpisodeUpgrade
        );

        slotv2::bonus_cycle::Result upgrade{};
        upgrade.outcome =
            slotv2::bonus_cycle::Outcome::EpisodeUpgradePending;

        const auto r = slotv2::bonus_transition::finalize(
            machine,
            pending,
            upgrade
        );

        ok = ok && r.outcome
            == slotv2::bonus_transition::Outcome::EpisodeStarted;
        ok = ok && machine.area == slotv2::machine_state::Area::Bonus;
        ok = ok && machine.bonus.active;
        ok = ok && machine.bonus.kind == slotv2::bonus_state::Kind::Episode;
        ok = ok && machine.bonus.medals_left == 80;
        ok = ok && machine.bonus_return_valid;
        ok = ok && machine.bonus_return_area
            == slotv2::machine_state::Area::Normal;
        ok = ok && !slotv2::pending_event::has(
            pending,
            slotv2::pending_event::BonusEpisodeUpgrade
        );
    }

    if (!ok) {
        std::cerr << "slot_v2_bonus_transition_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_bonus_transition_test: OK\n";
    return 0;
}
