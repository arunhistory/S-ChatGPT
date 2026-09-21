#include "fallback.hpp"

namespace slotv2::stop_rules::fallback {

bool accepts(const stop_shared::Context& ctx, const reel_strip::StripView& strip, uint8_t candidate) {
    // 役専用制御未実装の段階でも、非チェリー成立時の左🍒表示禁止だけは必ず守る。
    return leftCherrySafe(ctx, strip, candidate);
}

} // namespace slotv2::stop_rules::fallback
