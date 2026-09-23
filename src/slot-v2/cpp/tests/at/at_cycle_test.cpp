#include <iostream>
#include "at/at_cycle.hpp"
#include "at/at_event.hpp"
#include "core/machine_state.hpp"

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

    for (int i = 0; i < 100; ++i) {
        const auto r = slotv2::at_cycle::beginGame(rng, machine);
        ok = ok && r.active;
        ok = ok && r.games_left_before == 100 - i;
        ok = ok && r.games_left_after == 99 - i;
        ok = ok && r.window_empty_after_game == (i == 99);
        const uint32_t b = r.raw.bits;
        ok = ok && (b == 0u || (b & (b - 1u)) == 0u);
    }

    ok = ok && machine.at.games_left == 0;
    ok = ok && machine.at.active;

    {
        const auto empty = slotv2::at_cycle::beginGame(rng, machine);
        ok = ok && empty.active;
        ok = ok && empty.raw.bits == 0u;
        ok = ok && empty.window_empty_after_game;
    }

    uint32_t seen = 0u;
    for (uint64_t i = 0; i < 6000000ULL; ++i) {
        if (machine.at.games_left <= 0) {
            slotv2::at_state::start(machine.at, slotv2::at_state::Tier::Lower);
            machine.at.table = (i & 1u)
                ? slotv2::at_state::Table::Heaven
                : slotv2::at_state::Table::Specialized;
        }

        const auto r = slotv2::at_cycle::beginGame(rng, machine);
        const uint32_t b = r.raw.bits;
        ok = ok && (b == 0u || (b & (b - 1u)) == 0u);
        seen |= b;
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
