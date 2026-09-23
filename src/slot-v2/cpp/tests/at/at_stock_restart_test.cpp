#include <iostream>
#include "at/at_stock_restart.hpp"
#include "at/at_table_transition.hpp"

int main() {
    bool ok = true;

    uint64_t total = 0u;
    for (uint16_t i = 0; i < 1000u; ++i) {
        total += static_cast<uint64_t>(
            slotv2::at_stock_restart::gamesFromRoll(i)
        );
    }
    ok = ok && total == 49250u;

    uint32_t normal=0, heaven=0, super=0, specialized=0;
    for (uint16_t i = 0; i < 100u; ++i) {
        const auto t = slotv2::at_table_transition::fromRoll(
            slotv2::at_state::Table::Heaven,
            i
        );
        normal += t == slotv2::at_state::Table::Normal;
        heaven += t == slotv2::at_state::Table::Heaven;
        super += t == slotv2::at_state::Table::SuperHeaven;
        specialized += t == slotv2::at_state::Table::Specialized;
    }

    ok = ok
        && normal == 30u
        && heaven == 25u
        && super == 20u
        && specialized == 25u;

    if (!ok) {
        std::cerr << "slot_v2_at_stock_restart_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_at_stock_restart_test: OK\n";
    return 0;
}
