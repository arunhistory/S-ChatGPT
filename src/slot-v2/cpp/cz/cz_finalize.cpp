#include "index.hpp"
#include "../progress-event/index.hpp"

namespace slotv2::cz_finalize {

Result apply(
    machine_state::State& machine,
    pending_event::State& pending,
    const cz_cycle::Result& cycle
) {
    if (!cycle.active) return {};

    if (cycle.base_hit) {
        cz_state::resolve(machine.cz);
        (void)progress_event::onCZResolved(
            machine.normal_progress,
            pending,
            true
        );
        pending_event::add(pending, pending_event::CZHit);

        return {
            Outcome::HitPending,
            true
        };
    }

    if (!cycle.ended) return {};

    const bool three_miss = progress_event::onCZResolved(
        machine.normal_progress,
        pending,
        false
    );

    if (three_miss) {
        return {
            Outcome::ThreeMissHitPending,
            true
        };
    }

    machine.area = machine_state::Area::Normal;
    return {
        Outcome::MissReturnNormal,
        false
    };
}

} // namespace slotv2::cz_finalize
