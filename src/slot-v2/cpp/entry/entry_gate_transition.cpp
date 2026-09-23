#include "entry/entry_gate_transition.hpp"
#include "reel/reel_line.hpp"
#include "bonus/bonus_transition.hpp"

namespace slotv2::entry_gate_transition {

Result apply(
    machine_state::State& machine,
    uint8_t left_pos,
    uint8_t middle_pos,
    uint8_t right_pos
) {
    if (!machine.entry_gate.active) return {};

    const auto line = line::read(
        left_pos,
        middle_pos,
        right_pos
    );
    if (!line.ready) return {};

    if (!entry_gate::matches(
            machine.entry_gate,
            line.left,
            line.middle,
            line.right
        )) {
        // Internal hit remains queued. A later game gets another 1/2 attempt.
        machine.entry_gate.armed_this_game = false;
        return {};
    }

    const auto queued = machine.entry_gate;

    if (queued.kind == entry_gate::Kind::Bonus) {
        const auto return_area = queued.bonus_return_to_at
            ? machine_state::Area::AT
            : machine_state::Area::Normal;

        bonus_transition::start(
            machine,
            queued.bonus_kind,
            return_area
        );
        entry_gate::clear(machine.entry_gate);
        return {
            Outcome::BonusStarted,
            0u
        };
    }

    if (queued.kind == entry_gate::Kind::AT) {
        at_state::start(machine.at, queued.at_tier);
        machine.area = machine_state::Area::AT;
        if (queued.stock_to_add > 0u) {
            stock::add(machine.stock, queued.stock_to_add);
        }
        entry_gate::clear(machine.entry_gate);
        return {
            Outcome::ATStarted,
            queued.stock_to_add
        };
    }

    return {};
}

} // namespace slotv2::entry_gate_transition
