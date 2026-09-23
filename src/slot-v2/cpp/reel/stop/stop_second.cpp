#include "index.hpp"
#include "../stop-first/index.hpp"
#include "../role-policy/index.hpp"
#include "../assist-integrity/index.hpp"

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
