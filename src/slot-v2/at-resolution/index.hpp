#pragma once
#include <stdint.h>
#include "../at-event/index.hpp"

namespace slotv2::at_resolution {

enum class Status : uint8_t {
    None = 0,
    Single = 1,
    Multiple = 2
};

enum class Event : uint8_t {
    None = 0,
    Hit = 1,
    Fall = 2,
    AddGames = 3,
    Special = 4,
    Episode = 5,
    UpperSpecial = 6
};

struct Result {
    Status status{Status::None};
    Event event{Event::None};
    uint8_t count{0};
};

// The current AT lottery uses one disjoint shared box, so Multiple must not
// occur in normal play. Keep the state only as an invariant/error guard for
// malformed externally constructed Draw/Event values and backwards-compatible
// snapshots.
Result classify(const at_event::Result& raw);

} // namespace slotv2::at_resolution
