#include <iostream>
#include "at-add-games/index.hpp"
#include "stock-restart-lottery/index.hpp"
#include "at-internal-transition/index.hpp"
#include "at-window-transition/index.hpp"

int main() {
    bool ok = true;

    uint32_t add_total = 0u;
    for (uint16_t i = 0; i < 100u; ++i) {
        add_total += slotv2::at_add_games::fromRoll(i);
    }
    ok = ok && add_total == 2440u;

    uint64_t restart_total = 0u;
    for (uint16_t i = 0; i < 2000u; ++i) {
        restart_total += slotv2::stock_restart_lottery::fromRoll(i);
    }
    ok = ok && restart_total == 98500u; // 49.25G * 2000

    slotv2::Rng rng(0xA771234ULL);

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);
        slotv2::at_state::start(machine.at, slotv2::at_state::Tier::Middle);
        machine.area = slotv2::machine_state::Area::AT;
        slotv2::pending_event::State pending{};
        slotv2::pending_event::add(pending, slotv2::pending_event::ATFall);

        const auto r = slotv2::at_internal_transition::apply(rng, machine, pending);
        ok = ok && r.fall_applied
            && machine.at.tier == slotv2::at_state::Tier::Lower
            && machine.at.games_left == 100;
    }

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);
        slotv2::at_state::start(machine.at, slotv2::at_state::Tier::Lower);
        machine.area = slotv2::machine_state::Area::AT;
        machine.at.games_left = 0;
        slotv2::stock::add(machine.stock, 1u);
        slotv2::pending_event::State pending{};
        slotv2::pending_event::add(pending, slotv2::pending_event::ATWindowEmpty);

        const auto r = slotv2::at_window_transition::apply(rng, machine, pending);
        ok = ok && r.outcome == slotv2::at_window_transition::Outcome::StockRestart
            && machine.at.games_left > 0
            && machine.stock.count == 0u;
    }

    if (!ok) {
        std::cerr << "slot_v2_at_transition_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_at_transition_test: OK\n";
    return 0;
}
