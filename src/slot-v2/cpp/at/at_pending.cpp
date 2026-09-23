#include "at/at_pending.hpp"

namespace slotv2::at_pending {

void publish(
    const at_resolution::Result& resolution,
    pending_event::State& pending
) {
    if (resolution.status == at_resolution::Status::None) return;

    if (resolution.status == at_resolution::Status::Multiple) {
        pending_event::add(pending, pending_event::ATMultiple);
        return;
    }

    switch (resolution.event) {
        case at_resolution::Event::Hit:
            pending_event::add(pending, pending_event::ATHit);
            break;
        case at_resolution::Event::Fall:
            pending_event::add(pending, pending_event::ATFall);
            break;
        case at_resolution::Event::AddGames:
            pending_event::add(pending, pending_event::ATAddGames);
            break;
        case at_resolution::Event::Special:
            pending_event::add(pending, pending_event::ATSpecial);
            break;
        case at_resolution::Event::Episode:
            pending_event::add(pending, pending_event::ATEpisode);
            break;
        case at_resolution::Event::UpperSpecial:
            pending_event::add(pending, pending_event::ATUpperSpecial);
            break;
        case at_resolution::Event::None:
        default:
            break;
    }
}

} // namespace slotv2::at_pending
