#include "reel/stop/rules/strong_chance_rule.hpp"

namespace slotv2::stop_rules::strong_chance {

bool accepts(
    const stop_shared::Context& ctx,
    const reel_strip::StripView& strip,
    uint8_t candidate
) {
    if (!leftCherrySafe(ctx, strip, candidate)) return false;

    // Primary strong-chance result: middle line Penguin / Cherry / Penguin.
    // If the eye-stop misses, the generic substitute fallback handles it.
    switch (ctx.reel) {
        case ReelId::Left:
        case ReelId::Right:
            return centerIs(strip, candidate, Symbol::Penguin);
        case ReelId::Middle:
            return centerIs(strip, candidate, Symbol::Cherry);
    }
    return false;
}

} // namespace slotv2::stop_rules::strong_chance
