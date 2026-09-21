#include "watermelon.hpp"

namespace slotv2::stop_rules::watermelon {

bool accepts(const stop_shared::Context& ctx, const reel_strip::StripView& strip, uint8_t candidate) {
    if (!leftCherrySafe(ctx, strip, candidate)) return false;
    if (ctx.reel == ReelId::Left) return centerIs(strip, candidate, Symbol::Watermelon);

    // 中・右配列未確定中。仮停止形は作らず、配列確定後に役専用条件を追加する。
    return true;
}

} // namespace slotv2::stop_rules::watermelon
