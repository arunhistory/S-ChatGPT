#include "index.hpp"
#include "../assist-target/index.hpp"
#include "../reel-read/index.hpp"

namespace slotv2::assist_integrity {

bool stoppedCompatible(const stop_shared::Context& ctx) {
    if (ctx.role != RoleFlag::Bell9
        && ctx.role != RoleFlag::Bell15
        && ctx.role != RoleFlag::Replay) {
        return true;
    }

    for (uint8_t i = 0; i < 3u; ++i) {
        if (!ctx.stopped[i]) continue;

        const auto reel = static_cast<ReelId>(i);
        const auto symbol = reel_read::at(reel, ctx.stopped_position[i]);
        if (symbol == Symbol::Unknown) return false;
        if (!assist_target::accepts(ctx.role, reel, symbol)) return false;
    }

    return true;
}

} // namespace slotv2::assist_integrity
