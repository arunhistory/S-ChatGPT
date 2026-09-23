#include "at/at_resolution.hpp"

namespace slotv2::at_resolution {

Result classify(const at_event::Result& raw) {
    Result out{};

    const struct Pair {
        at_event::Bits bit;
        Event event;
    } pairs[] = {
        {at_event::Hit, Event::Hit},
        {at_event::Fall, Event::Fall},
        {at_event::AddGames, Event::AddGames},
        {at_event::Special, Event::Special},
        {at_event::Episode, Event::Episode},
        {at_event::UpperSpecial, Event::UpperSpecial},
    };

    for (const auto& pair : pairs) {
        if (!at_event::has(raw, pair.bit)) continue;
        ++out.count;
        if (out.count == 1u) {
            out.event = pair.event;
        }
    }

    if (out.count == 0u) {
        out.status = Status::None;
        out.event = Event::None;
    } else if (out.count == 1u) {
        out.status = Status::Single;
    } else {
        out.status = Status::Multiple;
        out.event = Event::None;
    }

    return out;
}

} // namespace slotv2::at_resolution
