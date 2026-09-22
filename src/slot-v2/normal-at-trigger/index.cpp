#include "index.hpp"
#include "../progress-event/index.hpp"

namespace slotv2::normal_at_trigger {

namespace {

void startLowerAT(machine_state::State& machine) {
    at_state::start(
        machine.at,
        at_state::Tier::Lower
    );
    machine.area = machine_state::Area::AT;
}

}

Result applyBellFive(
    machine_state::State& machine,
    pending_event::State& pending
) {
    if (machine.area != machine_state::Area::Normal) return {};
    if (!pending_event::has(pending, pending_event::BellFiveAT)) return {};

    startLowerAT(machine);

    (void)pending_event::consume(
        pending,
        pending_event::BellFiveAT
    );

    return {
        true,
        true,
        false
    };
}

Result applyNextHitGuarantee(
    machine_state::State& machine,
    pending_event::State& pending,
    bool base_was_at
) {
    if (machine.area != machine_state::Area::Normal) return {};
    if (base_was_at) return {};
    if (!machine.normal_progress.next_hit_at_guaranteed) return {};

    if (!progress_event::consumeNextHitAT(
            machine.normal_progress,
            pending
        )) {
        return {};
    }

    startLowerAT(machine);

    return {
        true,
        false,
        true
    };
}

} // namespace slotv2::normal_at_trigger
