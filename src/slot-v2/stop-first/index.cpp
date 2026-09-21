#include "index.hpp"
#include "../stop-candidate/index.hpp"
#include "../reel-strip/index.hpp"
#include "../stop-rules/router.hpp"

namespace slotv2::stop_first {

stop_shared::Result resolve(const stop_shared::Context& ctx) {
    const auto strip = reel_strip::get(ctx.reel);
    if (!strip.data || strip.size == 0) {
        return {stop_shared::ResolveStatus::StripUndefined, ctx.pressed_position, 0};
    }

    const auto candidates = stop_candidate::build(ctx.pressed_position);
    for (uint8_t i = 0; i < candidates.count; ++i) {
        const uint8_t candidate = candidates.position[i];
        if (stop_rules::accepts(ctx, strip, candidate)) {
            return {stop_shared::ResolveStatus::Ok, candidate, i};
        }
    }

    // 目押し役は0〜4コマに届かなければ取りこぼし。
    // 非成立チェリーを表示して帳尻を合わせる処理は行わない。
    return {stop_shared::ResolveStatus::NoLegalCandidate, ctx.pressed_position, 0};
}

} // namespace slotv2::stop_first
