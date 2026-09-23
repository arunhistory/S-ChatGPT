#include "reel/stop/rules/miss_rule.hpp"

namespace slotv2::stop_rules::miss {

bool accepts(const stop_shared::Context& ctx, const reel_strip::StripView& strip, uint8_t candidate) {
    return leftCherrySafe(ctx, strip, candidate);
}

} // namespace slotv2::stop_rules::miss
