#include "reel/stop/rules/one_medal_rule.hpp"

namespace slotv2::stop_rules::one_medal {

bool accepts(const stop_shared::Context& ctx, const reel_strip::StripView& strip, uint8_t candidate) {
    // 1枚役の具体停止形は後続で詰める。
    // 現段階では非成立チェリーを見せない基本制御だけ適用。
    return leftCherrySafe(ctx, strip, candidate);
}

} // namespace slotv2::stop_rules::one_medal
