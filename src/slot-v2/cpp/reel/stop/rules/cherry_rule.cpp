#include "cherry.hpp"

namespace slotv2::stop_rules::cherry {

bool accepts(const stop_shared::Context& ctx, const reel_strip::StripView& strip, uint8_t candidate) {
    if (ctx.reel != ReelId::Left || !strip.data || strip.size == 0) return true;

    if (ctx.role == RoleFlag::StrongCherry) {
        return centerIs(strip, candidate, Symbol::Cherry);
    }

    if (ctx.role == RoleFlag::WeakCherry) {
        const uint8_t lower = static_cast<uint8_t>((candidate + 1u) % strip.size);
        return strip.data[lower] == Symbol::Cherry;
    }

    return false;
}

} // namespace slotv2::stop_rules::cherry
