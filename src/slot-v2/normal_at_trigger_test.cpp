#include <iostream>
#include "normal-at-trigger/index.hpp"

int main() {
    bool ok = true;

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);
        slotv2::pending_event::State pending{};

        slotv2::pending_event::add(
            pending,
            slotv2::pending_event::BellFiveAT
        );

        const auto r = slotv2::normal_at_trigger::applyBellFive(
            machine,
            pending
        );

        ok = ok && r.started && r.from_bell_five;
        ok = ok && machine.area == slotv2::machine_state::Area::AT;
        ok = ok && machine.at.active;
        ok = ok && machine.at.tier == slotv2::at_state::Tier::Lower;
        ok = ok && machine.at.games_left == 100;
        ok = ok && !slotv2::pending_event::has(
            pending,
            slotv2::pending_event::BellFiveAT
        );
    }

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);
        slotv2::pending_event::State pending{};

        machine.normal_progress.next_hit_at_guaranteed = true;
        slotv2::pending_event::add(
            pending,
            slotv2::pending_event::NextHitAT
        );

        const auto r =
            slotv2::normal_at_trigger::applyNextHitGuarantee(
                machine,
                pending,
                false
            );

        ok = ok && r.started && r.from_next_hit_guarantee;
        ok = ok && machine.area == slotv2::machine_state::Area::AT;
        ok = ok && !machine.normal_progress.next_hit_at_guaranteed;
        ok = ok && !slotv2::pending_event::has(
            pending,
            slotv2::pending_event::NextHitAT
        );
    }

    if (!ok) {
        std::cerr << "slot_v2_normal_at_trigger_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_normal_at_trigger_test: OK\n";
    return 0;
}
