#include "index.hpp"

namespace slotv2::at_single_transition {

Result apply(
    machine_state::State& machine,
    pending_event::State& pending,
    const at_resolution::Result& resolution
) {
    if (resolution.status != at_resolution::Status::Single) {
        return {};
    }

    if (resolution.event != at_resolution::Event::Special) {
        return {};
    }

    if (machine.area != machine_state::Area::AT
        || !machine.at.active
        || machine.special_zone.active) {
        return {};
    }

    special_zone::start(machine.special_zone);

    (void)pending_event::consume(
        pending,
        pending_event::ATSpecial
    );

    return {
        true,
        true
    };
}

} // namespace slotv2::at_single_transition
