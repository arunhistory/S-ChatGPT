#include "router.hpp"
#include "bell.hpp"
#include "replay.hpp"
#include "cherry.hpp"
#include "watermelon.hpp"
#include "fallback.hpp"

namespace slotv2::stop_rules {

bool accepts(const stop_shared::Context& ctx, const reel_strip::StripView& strip, uint8_t candidate) {
    switch (ctx.role) {
        case RoleFlag::Bell9:
        case RoleFlag::Bell15:
            return bell::accepts(ctx, strip, candidate);

        case RoleFlag::Replay:
            return replay::accepts(ctx, strip, candidate);

        case RoleFlag::WeakCherry:
        case RoleFlag::StrongCherry:
            return cherry::accepts(ctx, strip, candidate);

        case RoleFlag::Watermelon:
            return watermelon::accepts(ctx, strip, candidate);

        default:
            return fallback::accepts(ctx, strip, candidate);
    }
}

} // namespace slotv2::stop_rules
