#include <iostream>
#include "entry-gate/index.hpp"
#include "entry-gate-transition/index.hpp"
#include "machine-state/index.hpp"

int main() {
    bool ok = true;

    {
        slotv2::entry_gate::State state{};
        slotv2::entry_gate::queueAT(
            state,
            slotv2::at_state::Tier::Lower
        );
        state.armed_this_game = true;
        ok = ok && slotv2::entry_gate::roleForArmed(state)
            == slotv2::RoleFlag::EntryAT;
        ok = ok && slotv2::entry_gate::matches(
            state,
            slotv2::Symbol::Red7,
            slotv2::Symbol::Red7,
            slotv2::Symbol::Red7
        );
    }

    {
        slotv2::entry_gate::State state{};
        slotv2::entry_gate::queueBonus(
            state,
            slotv2::bonus_state::Kind::Regular,
            false
        );
        state.armed_this_game = true;
        ok = ok && slotv2::entry_gate::roleForArmed(state)
            == slotv2::RoleFlag::EntryBonus;
        ok = ok && slotv2::entry_gate::matches(
            state,
            slotv2::Symbol::Red7,
            slotv2::Symbol::Red7,
            slotv2::Symbol::Bar
        );
        ok = ok && !slotv2::entry_gate::matches(
            state,
            slotv2::Symbol::Red7,
            slotv2::Symbol::Red7,
            slotv2::Symbol::Red7
        );
    }

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);
        slotv2::entry_gate::queueAT(
            machine.entry_gate,
            slotv2::at_state::Tier::Lower,
            1u
        );
        machine.entry_gate.armed_this_game = true;

        const auto r = slotv2::entry_gate_transition::apply(
            machine,
            4u, 8u, 11u
        );

        ok = ok && r.outcome == slotv2::entry_gate_transition::Outcome::ATStarted;
        ok = ok && machine.area == slotv2::machine_state::Area::AT;
        ok = ok && machine.at.active;
        ok = ok && machine.at.tier == slotv2::at_state::Tier::Lower;
        ok = ok && machine.at.games_left == 100;
        ok = ok && machine.stock.count == 1u;
        ok = ok && !machine.entry_gate.active;
    }

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);
        slotv2::entry_gate::queueBonus(
            machine.entry_gate,
            slotv2::bonus_state::Kind::Regular,
            false
        );
        machine.entry_gate.armed_this_game = true;

        const auto r = slotv2::entry_gate_transition::apply(
            machine,
            4u, 8u, 12u
        );

        ok = ok && r.outcome == slotv2::entry_gate_transition::Outcome::BonusStarted;
        ok = ok && machine.area == slotv2::machine_state::Area::Bonus;
        ok = ok && machine.bonus.active;
        ok = ok && machine.bonus.medals_left == 50;
        ok = ok && !machine.entry_gate.active;
    }

    if (!ok) {
        std::cerr << "slot_v2_entry_gate_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_entry_gate_test: OK\n";
    return 0;
}
