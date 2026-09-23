#include <iostream>
#include "normal/normal_hit_entry.hpp"

int main() {
    bool ok = true;

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);

        const auto r = slotv2::normal_hit_entry::enterBonus(
            machine,
            slotv2::normal_mode::Mode::NormalA
        );

        ok = ok && r.outcome == slotv2::normal_hit_entry::Outcome::Bonus;
        ok = ok && machine.area == slotv2::machine_state::Area::Normal;
        ok = ok && !machine.bonus.active;
        ok = ok && machine.entry_gate.active;
        ok = ok && machine.entry_gate.kind == slotv2::entry_gate::Kind::Bonus;
        ok = ok && machine.entry_gate.bonus_kind == slotv2::bonus_state::Kind::Regular;
    }

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);

        const auto r = slotv2::normal_hit_entry::enterBonus(
            machine,
            slotv2::normal_mode::Mode::Heaven
        );

        ok = ok && r.outcome == slotv2::normal_hit_entry::Outcome::Bonus;
        ok = ok && machine.entry_gate.active;
        ok = ok && machine.entry_gate.bonus_kind == slotv2::bonus_state::Kind::Episode;
        ok = ok && !machine.bonus.active;
    }

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);

        const auto r = slotv2::normal_hit_entry::enterBonus(
            machine,
            slotv2::normal_mode::Mode::Special
        );

        ok = ok && r.outcome == slotv2::normal_hit_entry::Outcome::Bonus;
        ok = ok && machine.area == slotv2::machine_state::Area::Normal;
        ok = ok && !machine.bonus.active;
        ok = ok && machine.entry_gate.active;
        ok = ok && machine.entry_gate.kind == slotv2::entry_gate::Kind::Bonus;
        ok = ok && machine.entry_gate.bonus_kind == slotv2::bonus_state::Kind::Regular;
    }

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);

        const auto r = slotv2::normal_hit_entry::enterAT(machine);

        ok = ok && r.outcome == slotv2::normal_hit_entry::Outcome::LowerAT;
        ok = ok && machine.area == slotv2::machine_state::Area::Normal;
        ok = ok && !machine.at.active;
        ok = ok && machine.entry_gate.active;
        ok = ok && machine.entry_gate.kind == slotv2::entry_gate::Kind::AT;
        ok = ok && machine.entry_gate.at_tier == slotv2::at_state::Tier::Lower;
    }

    if (!ok) {
        std::cerr << "slot_v2_normal_hit_entry_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_normal_hit_entry_test: OK\n";
    return 0;
}
