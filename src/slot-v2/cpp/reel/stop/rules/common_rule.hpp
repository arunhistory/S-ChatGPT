#pragma once
#include "reel/reel_strip.hpp"
#include "reel/stop/stop_types.hpp"

namespace slotv2::stop_rules {

bool visibleHas(const reel_strip::StripView& strip, uint8_t center, Symbol symbol);
bool centerIs(const reel_strip::StripView& strip, uint8_t center, Symbol symbol);
bool effectiveLineWatermelonSafe(
    const stop_shared::Context& ctx,
    const reel_strip::StripView& strip,
    uint8_t candidate
);
bool leftCherrySafe(const stop_shared::Context& ctx, const reel_strip::StripView& strip, uint8_t candidate);

// True only when this stop would complete a reserved middle-line result.
// Used by missed/substitute fallback so a miss never turns into another role.
bool completesReservedLine(
    const stop_shared::Context& ctx,
    const reel_strip::StripView& strip,
    uint8_t candidate
);

// True if the third stop would visually complete BELL-BELL-BELL on any
// visible cross-reel line: top, middle, bottom, ↘ or ↗.
// Internal lottery still decides the role; non-bell roles must avoid a
// misleading bell line instead of changing a 1-medal/miss into bell.
bool completesVisibleBellLine(
    const stop_shared::Context& ctx,
    const reel_strip::StripView& strip,
    uint8_t candidate
);

} // namespace slotv2::stop_rules
