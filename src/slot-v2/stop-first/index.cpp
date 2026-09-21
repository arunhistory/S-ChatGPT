#include "index.hpp"
#include "../stop-candidate/index.hpp"
#include "../reel-strip/index.hpp"
#include "../stop-rules/router.hpp"
#include "../stop-rules/fallback.hpp"

namespace slotv2::stop_first {

stop_shared::Result resolve(const stop_shared::Context& ctx) {
    const auto strip = reel_strip::get(ctx.reel);
    if (!strip.data || strip.size == 0) {
        return {stop_shared::ResolveStatus::StripUndefined, ctx.pressed_position, 0};
    }

    const auto candidates = stop_candidate::build(ctx.pressed_position);

    // まず成立役そのものを0〜4コマで引き込めるか判定。
    for (uint8_t i = 0; i < candidates.count; ++i) {
        const uint8_t candidate = candidates.position[i];
        if (stop_rules::accepts(ctx, strip, candidate)) {
            return {stop_shared::ResolveStatus::Ok, candidate, i};
        }
    }

    // 目押し役を取りこぼした場合もリールは必ず0〜4コマ内で止める。
    // 非チェリー成立なら、取りこぼし停止でも左可視3コマへ🍒は出さない。
    for (uint8_t i = 0; i < candidates.count; ++i) {
        const uint8_t candidate = candidates.position[i];
        if (stop_rules::fallback::accepts(ctx, strip, candidate)) {
            return {stop_shared::ResolveStatus::RoleMissed, candidate, i};
        }
    }

    // チェリーフラグ成立時は🍒表示自体を禁止しないため、押下位置で停止可能。
    if (ctx.role == RoleFlag::WeakCherry || ctx.role == RoleFlag::StrongCherry) {
        return {stop_shared::ResolveStatus::RoleMissed, candidates.position[0], 0};
    }

    return {stop_shared::ResolveStatus::NoLegalCandidate, ctx.pressed_position, 0};
}

} // namespace slotv2::stop_first
