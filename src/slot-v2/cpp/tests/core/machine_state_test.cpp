#include <iostream>
#include "core/machine_state.hpp"
#include "core/accounting.hpp"
#include "core/point_ledger.hpp"

int main() {
    bool ok = true;

    slotv2::machine_state::State machine{};
    slotv2::machine_state::reset(machine);
    ok = ok && machine.area == slotv2::machine_state::Area::Normal;

    slotv2::accounting::State accounting{};
    auto bet = slotv2::accounting::debit(accounting, machine.section, 3);
    ok = ok && bet.total_bet == 3 && bet.total_diff == -3;

    auto payout = slotv2::accounting::credit(accounting, machine.section, 9);
    ok = ok && payout.total_payout == 9 && payout.total_diff == 6;

    slotv2::point_ledger::State points{};
    slotv2::point_ledger::add(points, 10);
    ok = ok && slotv2::point_ledger::consume(points, 7);
    ok = ok && points.points == 3;
    ok = ok && !slotv2::point_ledger::consume(points, 4);

    if (!ok) {
        std::cerr << "slot_v2_machine_state_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_machine_state_test: OK\n";
    return 0;
}
