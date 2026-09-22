#include <iostream>
#include "machine-state/index.hpp"
#include "special-result/index.hpp"
#include "special-apply/index.hpp"

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

        ok = ok && r.applied && r.at_started;
        ok = ok && machine.area == slotv2::machine_state::Area::AT;
        ok = ok && machine.at.tier == slotv2::at_state::Tier::Middle;
        ok = ok && machine.at.games_left == 100;
        ok = ok && machine.stock.count >= 1 && machine.stock.count <= 5;
        ok = ok && r.stock_added == machine.stock.count;
    }

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);

        const auto special = slotv2::special_result::resolve(
            slotv2::SpecialHit::UpperAT
        );
        (void)slotv2::special_apply::apply(rng, machine, special);

        ok = ok && machine.at.tier == slotv2::at_state::Tier::Upper;
        ok = ok && machine.stock.count == 0;
    }

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);

        const auto special = slotv2::special_result::resolve(
            slotv2::SpecialHit::Freeze
        );
        const auto r = slotv2::special_apply::apply(rng, machine, special);

        ok = ok && r.freeze;
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
