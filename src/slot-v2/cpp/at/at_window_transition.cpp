#include "at/at_window_transition.hpp"

namespace slotv2::at_window_transition {

Result apply(
    Rng&,
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

    // Stock restart belongs exclusively to at-stock-restart.
    if (machine.stock.count > 0u) {
        return {};
    }

    const auto ended_tier = machine.at.tier;

    (void)pending_event::consume(pending, pending_event::ATWindowEmpty);
    (void)pending_event::consume(pending, pending_event::ATStockAvailable);

    at_state::end(machine.at);

    if (ended_tier == at_state::Tier::Upper) {
        machine.area = machine_state::Area::Normal;
        upper_comeback::start(machine.upper_comeback);
        return {
            Outcome::UpperComeback,
            ended_tier
        };
    }

    revival_state::start(machine.revival, ended_tier);
    machine.area = machine_state::Area::Revival;
    return {
        Outcome::Revival,
        ended_tier
    };
}

} // namespace slotv2::at_window_transition
