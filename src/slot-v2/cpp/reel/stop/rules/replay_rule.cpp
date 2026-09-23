#include "replay.hpp"
#include "../assist-target/index.hpp"

namespace slotv2::stop_rules::replay {

bool accepts(const stop_shared::Context& ctx, const reel_strip::StripView& strip, uint8_t candidate) {
    if (!leftCherrySafe(ctx, strip, candidate)) return false;
    const auto symbol = strip.data[candidate % strip.size];
    return assist_target::accepts(ctx.role, ctx.reel, symbol);
}

} // namespace slotv2::stop_rules::replay
