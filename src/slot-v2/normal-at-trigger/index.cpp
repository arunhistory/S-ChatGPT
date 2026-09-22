#include "index.hpp"
#include "../progress-event/index.hpp"

namespace slotv2::normal_at_trigger {

namespace {

void queueLowerAT(machine_state::State& machine) {
    entry_gate::queueAT(
        machine.entry_gate,
        at_state::Tier::Lower
    );
}

}

Result applyBellFive(
    machine_state::State& machine,
    pending_event::State& pending
) {
    if (machine.area != machine_state::Area::Normal) return {};
    if (!pending_event::has(pending, pending_event::BellFiveAT)) return {};

    queueLowerAT(machine);

    (void)pending_event::consume(
        pending,
        pending_event::BellFiveAT
    );

    return {
        true,
        false,
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
    if (!machine.normal_progress.next_hit_at_guaranteed) return {};

    if (!progress_event::consumeNextHitAT(
            machine.normal_progress,
            pending
        )) {
        return {};
    }

    if (!base_was_at) {
        queueLowerAT(machine);
    }

    return {
        !base_was_at,
        true,
        false,
        true
    };
}

} // namespace slotv2::normal_at_trigger
