#include "reel/stop/stop_third.hpp"
#include "reel/stop/stop_first.hpp"
#include "reel/stop/role_policy.hpp"
#include "reel/stop/assist_integrity.hpp"

namespace slotv2::stop_third {

stop_shared::Result resolve(const stop_shared::Context& ctx) {
    const auto result = stop_first::resolve(ctx);

    if (role_policy::stopPolicy(ctx.role) != role_policy::StopPolicy::Assist) {
        return result;
    }

    // 最終停止でも、前停止で崩れたアシスト役を別結果へ書き換えない。
    if (!assist_integrity::stoppedCompatible(ctx)
        && (result.status == stop_shared::ResolveStatus::Ok
            || result.status == stop_shared::ResolveStatus::AssistGap)) {
        return {
            stop_shared::ResolveStatus::AssistGap,
            result.final_position,
            result.slip
        };
    }

    return result;
}

} // namespace slotv2::stop_third
