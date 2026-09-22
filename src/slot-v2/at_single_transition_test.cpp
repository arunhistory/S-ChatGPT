#include <iostream>
#include "at-single-transition/index.hpp"

int main() {
    bool ok = true;

    slotv2::machine_state::State machine{};
    slotv2::machine_state::reset(machine);
    slotv2::at_state::start(
        machine.at,
        slotv2::at_state::Tier::Lower
    );
    machine.area = slotv2::machine_state::Area::AT;

    slotv2::pending_event::State pending{};
    slotv2::pending_event::add(
        pending,
        slotv2::pending_event::ATSpecial
    );

    slotv2::at_resolution::Result special{
        slotv2::at_resolution::Status::Single,
        slotv2::at_resolution::Event::Special,
        1
    };

    const auto applied = slotv2::at_single_transition::apply(
        machine,
        pending,
        special
    );

    ok = ok && applied.applied && applied.special_started;
    ok = ok && machine.special_zone.active;
    ok = ok && machine.special_zone.games_left == 5u;
    ok = ok && !slotv2::pending_event::has(
        pending,
        slotv2::pending_event::ATSpecial
    );

    slotv2::pending_event::add(
        pending,
        slotv2::pending_event::ATSpecial
    );

    const auto duplicate = slotv2::at_single_transition::apply(
        machine,
        pending,
        special
    );

    ok = ok && !duplicate.applied;
    ok = ok && slotv2::pending_event::has(
        pending,
        slotv2::pending_event::ATSpecial
    );

    if (!ok) {
        std::cerr << "slot_v2_at_single_transition_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_at_single_transition_test: OK\n";
    return 0;
}
