#include "fallback.hpp"

namespace slotv2::stop_rules::fallback {

bool accepts(
    const stop_shared::Context& ctx,
    const reel_strip::StripView& strip,
    uint8_t candidate
) {
    if (!leftCherrySafe(ctx, strip, candidate)) return false;

    // A missed/substitute role must stop on a zero-payout non-result.
    return !completesReservedLine(ctx, strip, candidate);
}

} // namespace slotv2::stop_rules::fallback
