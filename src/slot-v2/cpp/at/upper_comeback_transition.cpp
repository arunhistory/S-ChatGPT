#include "at/upper_comeback_transition.hpp"

namespace slotv2::upper_comeback_transition {

Result apply(
    machine_state::State& machine,
    pending_event::State& pending,
    const upper_comeback_cycle::Result& cycle
) {
    if (!cycle.ended) return {};

    if (cycle.hit) {
        // Comeback hit is fixed here, but Upper AT does not start until
        // the red-777 start signal is actually aligned.
        entry_gate::queueAT(
            machine.entry_gate,
            at_state::Tier::Upper
        );
        (void)pending_event::consume(
            pending,
            pending_event::UpperComebackHit
        );
        return {Outcome::UpperAT};
    }

    revival_state::start(
        machine.revival,
        at_state::Tier::Upper
    );
    machine.area = machine_state::Area::Revival;
    return {Outcome::Revival};
}

} // namespace slotv2::upper_comeback_transition
