#include <iostream>
#include "special-zone/index.hpp"
#include "special-zone-transition/index.hpp"

int main() {
    bool ok = true;

    uint32_t total = 0u;
    for (uint16_t i = 0; i < 100u; ++i) {
        total += slotv2::special_zone::addGamesFromRoll(i);
    }
    ok = ok && total == 4100u;

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);
        slotv2::at_state::start(
            machine.at,
            slotv2::at_state::Tier::Lower
        );
        machine.area = slotv2::machine_state::Area::AT;
        machine.special_zone.pending_add_games = 50u;

        slotv2::pending_event::State pending{};
        slotv2::pending_event::add(
            pending,
            slotv2::pending_event::SpecialZoneAddGames
        );

        const auto r = slotv2::special_zone_transition::apply(
            machine,
            pending
        );

        ok = ok && r.outcome
            == slotv2::special_zone_transition::Outcome::AddGamesApplied;
        ok = ok && r.added_games == 50u;
        ok = ok && machine.at.games_left == 150;
        ok = ok && machine.special_zone.pending_add_games == 0u;
        ok = ok && !slotv2::pending_event::has(
            pending,
            slotv2::pending_event::SpecialZoneAddGames
        );
    }

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);
        slotv2::at_state::start(
            machine.at,
            slotv2::at_state::Tier::Middle
        );
        machine.area = slotv2::machine_state::Area::AT;

        slotv2::pending_event::State pending{};
        slotv2::pending_event::add(
            pending,
            slotv2::pending_event::SpecialZoneBonus
        );

        const auto r = slotv2::special_zone_transition::apply(
            machine,
            pending
        );

        ok = ok && r.outcome
            == slotv2::special_zone_transition::Outcome::BonusQueued;
        ok = ok && machine.entry_gate.active;
        ok = ok && machine.entry_gate.kind
            == slotv2::entry_gate::Kind::Bonus;
        ok = ok && machine.entry_gate.bonus_return_to_at;
        ok = ok && !machine.bonus.active;
        ok = ok && !slotv2::pending_event::has(
            pending,
            slotv2::pending_event::SpecialZoneBonus
        );
    }

    if (!ok) {
        std::cerr << "slot_v2_special_zone_transition_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_special_zone_transition_test: OK\n";
    return 0;
}
