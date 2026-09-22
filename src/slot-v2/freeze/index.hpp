#pragma once
#include "../shared/types.hpp"
#include "../stop-shared/index.hpp"

namespace slotv2::freeze {

struct Directive {
    bool active;
    Symbol target_symbol;
    bool special_presentation_required;
};

// Lottery belongs to special-lottery. Freeze owns only the forced BLUE777 control.
Directive begin(SpecialHit hit);

// Freeze is a special control exception to the ordinary 0-4 slip window.
// Pressed timing is accepted, but each reel is forced to its BLUE7 center position.
stop_shared::Result forceStop(
    ReelId reel,
    uint8_t pressed_position
);

} // namespace slotv2::freeze
