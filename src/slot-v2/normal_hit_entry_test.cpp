#include <iostream>
#include "normal-hit-entry/index.hpp"

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
        ok = ok && machine.area == slotv2::machine_state::Area::Bonus;
        ok = ok && machine.bonus.active;
        ok = ok && machine.bonus.kind == slotv2::bonus_state::Kind::Regular;
        ok = ok && machine.bonus.medals_left == 50;
    }

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);

        const auto r = slotv2::normal_hit_entry::enterBonus(
            machine,
            slotv2::normal_mode::Mode::Heaven
        );

        ok = ok && r.outcome == slotv2::normal_hit_entry::Outcome::Bonus;
        ok = ok && machine.bonus.kind == slotv2::bonus_state::Kind::Episode;
        ok = ok && machine.bonus.medals_left == 80;
    }

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);

        const auto r = slotv2::normal_hit_entry::enterBonus(
            machine,
            slotv2::normal_mode::Mode::Special
        );

        ok = ok && r.outcome
            == slotv2::normal_hit_entry::Outcome::UnresolvedSpecialMode;
        ok = ok && machine.area == slotv2::machine_state::Area::Normal;
    }

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);

        const auto r = slotv2::normal_hit_entry::enterAT(machine);

        ok = ok && r.outcome == slotv2::normal_hit_entry::Outcome::LowerAT;
        ok = ok && machine.area == slotv2::machine_state::Area::AT;
        ok = ok && machine.at.active;
        ok = ok && machine.at.tier == slotv2::at_state::Tier::Lower;
        ok = ok && machine.at.games_left == 100;
    }

    if (!ok) {
        std::cerr << "slot_v2_normal_hit_entry_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_normal_hit_entry_test: OK\n";
    return 0;
}
