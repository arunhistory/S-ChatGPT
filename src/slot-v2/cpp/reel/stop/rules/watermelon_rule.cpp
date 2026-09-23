#include "reel/stop/rules/watermelon_rule.hpp"

namespace slotv2::stop_rules::watermelon {

bool accepts(
    const stop_shared::Context& ctx,
    const reel_strip::StripView& strip,
    uint8_t candidate
) {
    if (!leftCherrySafe(ctx, strip, candidate)) return false;
    return centerIs(strip, candidate, Symbol::Watermelon);
}

} // namespace slotv2::stop_rules::watermelon
