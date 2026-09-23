#include "reel/stop/rules/entry_rule.hpp"

namespace slotv2::stop_rules::entry {

bool accepts(
    const stop_shared::Context& ctx,
    const reel_strip::StripView& strip,
    uint8_t candidate
) {
    if (!leftCherrySafe(ctx, strip, candidate)) return false;

    if (ctx.role == RoleFlag::EntryAT) {
        return centerIs(strip, candidate, Symbol::Red7);
    }

    if (ctx.role == RoleFlag::EntryBonus) {
        if (ctx.reel == ReelId::Right) {
            return centerIs(strip, candidate, Symbol::Bar);
        }
        return centerIs(strip, candidate, Symbol::Red7);
    }

    return false;
}

} // namespace slotv2::stop_rules::entry
