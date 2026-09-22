#include <iostream>
#include "at-cycle/index.hpp"
#include "at-event/index.hpp"
#include "machine-state/index.hpp"

int main() {
    bool ok = true;
    slotv2::Rng rng(0xA7C1E123ULL);

    slotv2::machine_state::State machine{};
    slotv2::machine_state::reset(machine);

    {
        const auto inactive = slotv2::at_cycle::beginGame(rng, machine);
        ok = ok && !inactive.active && inactive.raw.bits == 0u;
    }

    slotv2::at_state::start(machine.at, slotv2::at_state::Tier::Lower);
    machine.area = slotv2::machine_state::Area::AT;

    uint32_t seen = 0u;
    for (uint64_t i = 0; i < 5000000ULL; ++i) {
        const auto r = slotv2::at_cycle::beginGame(rng, machine);
        ok = ok && r.active;
        seen |= r.raw.bits;
    }

    const uint32_t required =
        slotv2::at_event::Hit |
        slotv2::at_event::Fall |
        slotv2::at_event::AddGames |
        slotv2::at_event::Special |
        slotv2::at_event::Episode |
        slotv2::at_event::UpperSpecial;

    ok = ok && (seen & required) == required;

    if (!ok) {
        std::cerr << "slot_v2_at_cycle_test: FAILED seen=" << seen << "\n";
        return 1;
    }

    std::cout << "slot_v2_at_cycle_test: OK seen=" << seen << "\n";
    return 0;
}
