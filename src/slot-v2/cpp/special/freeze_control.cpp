#include "index.hpp"
#include "../reel-strip/index.hpp"

namespace slotv2::freeze {

Directive begin(SpecialHit hit) {
    if (hit != SpecialHit::Freeze) {
        return {false, Symbol::Unknown, false};
    }
    return {true, Symbol::Blue7, true};
}

stop_shared::Result forceStop(
    ReelId reel,
    uint8_t pressed_position
) {
    const auto strip = reel_strip::get(reel);
    if (!strip.data || strip.size == 0u) {
        return {
            stop_shared::ResolveStatus::StripUndefined,
            pressed_position,
            0u
        };
    }

    for (uint8_t i = 0; i < strip.size; ++i) {
        if (strip.data[i] != Symbol::Blue7) continue;

        // Special freeze control is intentionally not restricted by kMaxSlip.
        // Expose the circular travel distance for animation/debug only.
        const uint8_t pressed = static_cast<uint8_t>(
            pressed_position % strip.size
        );
        const uint8_t travel = static_cast<uint8_t>(
            (pressed + strip.size - i) % strip.size
        );

        return {
            stop_shared::ResolveStatus::Ok,
            i,
            travel
        };
    }

    return {
        stop_shared::ResolveStatus::NoLegalCandidate,
        pressed_position,
        0u
    };
}

} // namespace slotv2::freeze
