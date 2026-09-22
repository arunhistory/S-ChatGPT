#include "index.hpp"
#include "../at-table-transition/index.hpp"

namespace slotv2::at_stock_restart {

int gamesFromRoll(uint16_t roll_0_to_999) {
    const uint16_t roll = static_cast<uint16_t>(roll_0_to_999 % 1000u);

    if (roll < 200u) return 20;   // 20%
    if (roll < 450u) return 30;   // 25%
    if (roll < 650u) return 40;   // 20%
    if (roll < 800u) return 50;   // 15%
    if (roll < 880u) return 75;   // 8%
    if (roll < 940u) return 100;  // 6%
    if (roll < 975u) return 150;  // 3.5%
    return 200;                   // 2.5%
}

Result apply(
    Rng& rng,
    machine_state::State& machine,
    pending_event::State& pending
) {
    if (machine.area != machine_state::Area::AT
        || !machine.at.active
        || machine.at.games_left > 0
        || !pending_event::has(pending, pending_event::ATWindowEmpty)
        || !pending_event::has(pending, pending_event::ATStockAvailable)
        || machine.stock.count == 0u) {
        return {};
    }

    if (!stock::consumeOne(machine.stock)) return {};

    const auto before = machine.at.table;
    const int games = gamesFromRoll(
        static_cast<uint16_t>(rng.uniformBelow(1000u))
    );
    const auto after = at_table_transition::draw(rng, before);

    machine.at.games_left = games;
    machine.at.table = after;

    (void)pending_event::consume(
        pending,
        pending_event::ATWindowEmpty
    );
    (void)pending_event::consume(
        pending,
        pending_event::ATStockAvailable
    );

    return {
        true,
        games,
        before,
        after,
        machine.stock.count
    };
}

} // namespace slotv2::at_stock_restart
