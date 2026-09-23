#include "at/at_window.hpp"

namespace slotv2::at_window {

Result inspect(const machine_state::State& machine) {
    if (machine.area != machine_state::Area::AT || !machine.at.active) {
        return {};
    }

    if (machine.at.games_left > 0) {
        return {
            Status::Running,
            machine.stock.count
        };
    }

    return {
        machine.stock.count > 0u
            ? Status::EmptyStockAvailable
            : Status::EmptyNoStock,
        machine.stock.count
    };
}

} // namespace slotv2::at_window
