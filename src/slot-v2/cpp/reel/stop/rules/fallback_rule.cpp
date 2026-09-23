#include "reel/stop/rules/fallback_rule.hpp"

namespace slotv2::stop_rules::fallback {

bool accepts(
    const stop_shared::Context& ctx,
    const reel_strip::StripView& strip,
    uint8_t candidate
) {
    if (!leftCherrySafe(ctx, strip, candidate)) return false;

    // A missed/substitute role must stop on a zero-payout non-result.
    // Also suppress visually complete bell lines on non-bell internal flags.
    if (ctx.role != RoleFlag::Bell9 && ctx.role != RoleFlag::Bell15
        && completesVisibleBellLine(ctx, strip, candidate)) {
        return false;
    }
    return !completesReservedLine(ctx, strip, candidate);
}

} // namespace slotv2::stop_rules::fallback
