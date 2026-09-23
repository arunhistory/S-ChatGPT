#include <iostream>
#include "core/machine_state.hpp"
#include "special/special_result.hpp"
#include "special/special_apply.hpp"

int main() {
    bool ok = true;
    slotv2::Rng rng(0x13579BDFULL);

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);

        const auto special = slotv2::special_result::resolve(
            slotv2::SpecialHit::MiddleATStock
        );
        const auto r = slotv2::special_apply::apply(rng, machine, special);

        ok = ok && r.applied && !r.at_started;
        ok = ok && machine.area == slotv2::machine_state::Area::Normal;
        ok = ok && !machine.at.active;
        ok = ok && machine.entry_gate.active;
        ok = ok && machine.entry_gate.at_tier == slotv2::at_state::Tier::Middle;
        ok = ok && machine.entry_gate.stock_to_add >= 1u
            && machine.entry_gate.stock_to_add <= 5u;
        ok = ok && machine.stock.count == 0u;
        ok = ok && r.stock_added == machine.entry_gate.stock_to_add;
    }

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);

        const auto special = slotv2::special_result::resolve(
            slotv2::SpecialHit::UpperAT
        );
        const auto r = slotv2::special_apply::apply(rng, machine, special);

        ok = ok && r.applied && !r.at_started;
        ok = ok && machine.entry_gate.active;
        ok = ok && machine.entry_gate.at_tier == slotv2::at_state::Tier::Upper;
        ok = ok && machine.stock.count == 0u;
    }

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);

        const auto special = slotv2::special_result::resolve(
            slotv2::SpecialHit::Freeze
        );
        const auto r = slotv2::special_apply::apply(rng, machine, special);

        ok = ok && r.freeze;
        ok = ok && r.at_started;
        ok = ok && machine.at.active;
        ok = ok && machine.at.tier == slotv2::at_state::Tier::Upper;
        ok = ok && machine.stock.count >= 1 && machine.stock.count <= 5;
        ok = ok && r.stock_added == machine.stock.count;
    }

    if (!ok) {
        std::cerr << "slot_v2_special_apply_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_special_apply_test: OK\n";
    return 0;
}
