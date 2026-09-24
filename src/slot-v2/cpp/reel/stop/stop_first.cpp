#include "reel/stop/stop_first.hpp"
#include "reel/stop/stop_candidate.hpp"
#include "reel/reel_strip.hpp"
#include "reel/stop/rules/rule_router.hpp"
#include "reel/stop/rules/fallback_rule.hpp"
#include "reel/stop/rules/common_rule.hpp"
#include "reel/stop/role_policy.hpp"

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
        if (!stop_rules::effectiveLineWatermelonSafe(ctx, strip, candidate)) continue;
        if (stop_rules::accepts(ctx, strip, candidate)) {
            return {stop_shared::ResolveStatus::Ok, candidate, i};
        }
    }

    // 成立役そのものを引き込めない場合も0〜4コマ内で必ず停止する。
    // 代用停止対応役は「取りこぼし」と混同せず SubstituteStop として返す。
    const auto policy = role_policy::stopPolicy(ctx.role);
    for (uint8_t i = 0; i < candidates.count; ++i) {
        const uint8_t candidate = candidates.position[i];
        if (!stop_rules::effectiveLineWatermelonSafe(ctx, strip, candidate)) continue;
        if (!stop_rules::fallback::accepts(ctx, strip, candidate)) continue;

        if (policy == role_policy::StopPolicy::SubstituteCapable) {
            return {stop_shared::ResolveStatus::SubstituteStop, candidate, i};
        }
        if (policy == role_policy::StopPolicy::EyeStop) {
            return {stop_shared::ResolveStatus::RoleMissed, candidate, i};
        }
        if (policy == role_policy::StopPolicy::Assist) {
            return {stop_shared::ResolveStatus::AssistGap, candidate, i};
        }
        return {stop_shared::ResolveStatus::NoLegalCandidate, candidate, i};
    }

    // チェリー成立時は🍒表示禁止の対象外なので、届かなければ純粋な目押しミス。
    if (policy == role_policy::StopPolicy::EyeStop) {
        return {stop_shared::ResolveStatus::RoleMissed, candidates.position[0], 0};
    }

    return {stop_shared::ResolveStatus::NoLegalCandidate, ctx.pressed_position, 0};
}

} // namespace slotv2::stop_first
