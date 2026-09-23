#include <iostream>
#include "normal/normal_ceiling_transition.hpp"

int main() {
    bool ok = true;

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);
        slotv2::pending_event::State pending{};

        const auto r = slotv2::normal_ceiling_transition::apply(
            machine,
            pending,
            slotv2::normal_mode::Mode::NormalA,
            slotv2::normal_ceiling::Reward::CZ
        );

        ok = ok && r.outcome
            == slotv2::normal_ceiling_transition::Outcome::CZStarted;
        ok = ok && machine.area == slotv2::machine_state::Area::CZ;
        ok = ok && machine.cz.active && machine.cz.games_left == 10u;
    }

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);
        slotv2::pending_event::State pending{};

        const auto r = slotv2::normal_ceiling_transition::apply(
            machine,
            pending,
            slotv2::normal_mode::Mode::NormalA,
            slotv2::normal_ceiling::Reward::Bonus
        );

        ok = ok && r.outcome
            == slotv2::normal_ceiling_transition::Outcome::BonusQueued;
        ok = ok && machine.entry_gate.active;
        ok = ok && machine.entry_gate.kind
            == slotv2::entry_gate::Kind::Bonus;
        ok = ok && !machine.bonus.active;
    }

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);
        slotv2::pending_event::State pending{};

        const auto r = slotv2::normal_ceiling_transition::apply(
            machine,
            pending,
            slotv2::normal_mode::Mode::Special,
            slotv2::normal_ceiling::Reward::LowerATWithStock
        );

        ok = ok && r.outcome
            == slotv2::normal_ceiling_transition::Outcome::ATWithStockQueued;
        ok = ok && machine.entry_gate.active;
        ok = ok && machine.entry_gate.kind
            == slotv2::entry_gate::Kind::AT;
        ok = ok && machine.entry_gate.at_tier
            == slotv2::at_state::Tier::Lower;
        ok = ok && machine.entry_gate.stock_to_add == 1u;
    }

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);
        slotv2::pending_event::State pending{};

        const auto r = slotv2::normal_ceiling_transition::apply(
            machine,
            pending,
            slotv2::normal_mode::Mode::Special,
            slotv2::normal_ceiling::Reward::Freeze
        );

        ok = ok && r.outcome
            == slotv2::normal_ceiling_transition::Outcome::FreezeQueued;
        ok = ok && machine.area == slotv2::machine_state::Area::Normal;
    }

    if (!ok) {
        std::cerr << "slot_v2_normal_ceiling_transition_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_normal_ceiling_transition_test: OK\n";
    return 0;
}
