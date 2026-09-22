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

    if (machine.area != machine_state::Area::AT
        || !machine.at.active) {
        return {};
    }

    switch (resolution.event) {
        case at_resolution::Event::Hit:
            if (machine.at_omen.active) return {};
            at_omen::start(machine.at_omen, false);
            (void)pending_event::consume(
                pending,
                pending_event::ATHit
            );
            return {true, true, false, false};

        case at_resolution::Event::Episode:
            if (machine.at_omen.active) return {};
            at_omen::start(machine.at_omen, true);
            (void)pending_event::consume(
                pending,
                pending_event::ATEpisode
            );
            return {true, false, true, false};

        case at_resolution::Event::Special:
            if (machine.special_zone.active) return {};

            special_zone::start(machine.special_zone);
            (void)pending_event::consume(
                pending,
                pending_event::ATSpecial
            );
            return {true, false, false, true};

        case at_resolution::Event::Fall:
        case at_resolution::Event::AddGames:
        case at_resolution::Event::UpperSpecial:
        case at_resolution::Event::None:
        default:
            return {};
    }
}

} // namespace slotv2::at_single_transition
