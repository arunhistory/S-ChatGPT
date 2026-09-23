#pragma once
#include "shared/types.hpp"

namespace slotv2::reel_strip {

struct StripView {
    const Symbol* data;
    uint8_t size;
};

// Final 21-symbol strips for left, middle and right reels.
StripView get(ReelId reel);

} // namespace slotv2::reel_strip
