#include "weak-chance.hpp"

namespace slotv2::stop_rules::weak_chance {

bool accepts(
    const stop_shared::Context& ctx,
    const reel_strip::StripView& strip,
    uint8_t candidate
) {
    if (!leftCherrySafe(ctx, strip, candidate)) return false;

    // Approved substitute: middle line Bell / Bell / Snow.
    switch (ctx.reel) {
        case ReelId::Left:
        case ReelId::Middle:
            return centerIs(strip, candidate, Symbol::Bell);
        case ReelId::Right:
            return centerIs(strip, candidate, Symbol::Snow);
    }
    return false;
}

} // namespace slotv2::stop_rules::weak_chance
