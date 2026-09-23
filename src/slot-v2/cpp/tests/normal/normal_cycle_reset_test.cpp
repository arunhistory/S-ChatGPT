#include <iostream>
#include "normal/normal_cycle_reset.hpp"

int main() {
    bool ok = true;

    slotv2::Rng rng(0xC1C1EULL);
    slotv2::machine_state::State machine{};
    slotv2::machine_state::reset(machine);

    machine.normal.actual_games = 777u;
    machine.normal.display_games = 900u;
    machine.normal_progress.bell9_streak = 4u;

    slotv2::normal_mode::Mode mode = slotv2::normal_mode::Mode::Special;
    slotv2::normal_route::State route{};
    route.pattern = 9u;
    route.ceiling = 777u;
    route.special_window_checked = true;
    route.ceiling_consumed = true;

    slotv2::normal_cycle_reset::apply(
        rng,
        machine,
        mode,
        route
    );

    ok = ok && machine.normal.actual_games == 0u;
    ok = ok && machine.normal.display_games == 0u;
    ok = ok && machine.normal_progress.bell9_streak == 0u;
    ok = ok && mode != slotv2::normal_mode::Mode::Special;
    ok = ok && route.pattern < 10u;
    ok = ok && route.ceiling > 0u;
    ok = ok && !route.special_window_checked;
    ok = ok && !route.ceiling_consumed;

    if (!ok) {
        std::cerr << "slot_v2_normal_cycle_reset_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_normal_cycle_reset_test: OK\n";
    return 0;
}
