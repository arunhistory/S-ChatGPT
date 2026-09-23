#include "reel/stop/stop_second.hpp"
#include "reel/stop/stop_first.hpp"
#include "reel/stop/role_policy.hpp"
#include "reel/stop/assist_integrity.hpp"

namespace slotv2::stop_second {

stop_shared::Result resolve(const stop_shared::Context& ctx) {
    const auto result = stop_first::resolve(ctx);

    if (role_policy::stopPolicy(ctx.role) != role_policy::StopPolicy::Assist) {
        return result;
    }

    // 第1停止ですでにアシスト成立条件を失っていた場合、
    // 第2停止で帳尻合わせせず AssistGap を維持する。
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

} // namespace slotv2::stop_second
