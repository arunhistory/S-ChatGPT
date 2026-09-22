#include "index.hpp"

namespace slotv2::upper_special_transition {

bool apply(
    machine_state::State& machine,
    pending_event::State& pending
) {
    const bool at_trigger = pending_event::has(
        pending,
        pending_event::ATUpperSpecial
    );
    const bool section_trigger = pending_event::has(
        pending,
        pending_event::SectionUpperSpec
    );

    if (!at_trigger && !section_trigger) return false;
    if (machine.area != machine_state::Area::AT || !machine.at.active) {
        return false;
    }
    if (machine.special_zone.active || machine.upper_special.active) {
        return false;
    }

    upper_special::start(machine.upper_special);

    if (at_trigger) {
        (void)pending_event::consume(
            pending,
            pending_event::ATUpperSpecial
        );
    }
    if (section_trigger) {
        (void)pending_event::consume(
            pending,
            pending_event::SectionUpperSpec
        );
    }

    return true;
}

} // namespace slotv2::upper_special_transition
