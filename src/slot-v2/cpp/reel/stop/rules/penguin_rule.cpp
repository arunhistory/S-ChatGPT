#include "reel/stop/rules/penguin_rule.hpp"

namespace slotv2::stop_rules::penguin {

bool accepts(
    const stop_shared::Context& ctx,
    const reel_strip::StripView& strip,
    uint8_t candidate
) {
    if (!leftCherrySafe(ctx, strip, candidate)) return false;
    return centerIs(strip, candidate, Symbol::Penguin);
}

} // namespace slotv2::stop_rules::penguin
