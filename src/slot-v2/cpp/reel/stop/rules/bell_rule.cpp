#include "reel/stop/rules/bell_rule.hpp"
#include "reel/stop/bell_shape.hpp"

namespace slotv2::stop_rules::bell {

bool accepts(
    const stop_shared::Context& ctx,
    const reel_strip::StripView& strip,
    uint8_t candidate
) {
    if (!leftCherrySafe(ctx, strip, candidate)) return false;
    return bell_shape::candidateCompatible(ctx, candidate);
}

} // namespace slotv2::stop_rules::bell
