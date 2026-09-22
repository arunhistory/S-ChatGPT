#include <iostream>
#include "entry-gate/index.hpp"

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

    if (!ok) {
        std::cerr << "slot_v2_entry_gate_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_entry_gate_test: OK\n";
    return 0;
}
