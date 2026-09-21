#include "bell.hpp"

namespace slotv2::stop_rules::bell {

bool accepts(const stop_shared::Context& ctx, const reel_strip::StripView& strip, uint8_t candidate) {
    if (!leftCherrySafe(ctx, strip, candidate)) return false;

    const auto symbol = strip.data[candidate % strip.size];

    // 右リールの🟥7はベル成立時だけベル代用として扱う。
    if (ctx.reel == ReelId::Right) {
        return symbol == Symbol::Bell || symbol == Symbol::Red7;
    }

    return symbol == Symbol::Bell;
}

} // namespace slotv2::stop_rules::bell
