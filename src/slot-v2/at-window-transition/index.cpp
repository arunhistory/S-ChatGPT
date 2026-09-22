#include "index.hpp"
#include "../stock-restart-lottery/index.hpp"

namespace slotv2::at_window_transition {

Result apply(
    Rng& rng,
    machine_state::State& machine,
    pending_event::State& pending
) {
    if (!pending_event::has(pending, pending_event::ATWindowEmpty)) {
        return {};
    }
    if (machine.area != machine_state::Area::AT || !machine.at.active) {
        return {};
    }
    if (machine.at.games_left > 0) {
        (void)pending_event::consume(pending, pending_event::ATWindowEmpty);
        (void)pending_event::consume(pending, pending_event::ATStockAvailable);
        return {};
    }

    const auto ended_tier = machine.at.tier;

    if (stock::consumeOne(machine.stock)) {
        const uint16_t games = stock_restart_lottery::draw(rng);
        machine.at.games_left = static_cast<int>(games);

        (void)pending_event::consume(pending, pending_event::ATWindowEmpty);
        (void)pending_event::consume(pending, pending_event::ATStockAvailable);

        return {
            Outcome::StockRestart,
            ended_tier,
            games
        };
    }

    (void)pending_event::consume(pending, pending_event::ATWindowEmpty);
    (void)pending_event::consume(pending, pending_event::ATStockAvailable);

    at_state::end(machine.at);

    if (ended_tier == at_state::Tier::Upper) {
        machine.area = machine_state::Area::Normal;
        upper_comeback::start(machine.upper_comeback);
        return {
            Outcome::UpperComeback,
            ended_tier,
            0u
        };
    }

    revival_state::start(machine.revival, ended_tier);
    machine.area = machine_state::Area::Revival;
    return {
        Outcome::Revival,
        ended_tier,
        0u
    };
}

} // namespace slotv2::at_window_transition
