#include "common.hpp"

namespace slotv2::stop_rules {

bool visibleHas(const reel_strip::StripView& strip, uint8_t center, Symbol symbol) {
    if (!strip.data || strip.size == 0) return false;
    const int n = strip.size;
    for (int row = -1; row <= 1; ++row) {
        int p = static_cast<int>(center) + row;
        while (p < 0) p += n;
        p %= n;
        if (strip.data[p] == symbol) return true;
    }
    return false;
}

bool centerIs(const reel_strip::StripView& strip, uint8_t center, Symbol symbol) {
    return strip.data && strip.size && strip.data[center % strip.size] == symbol;
}

bool leftCherrySafe(const stop_shared::Context& ctx, const reel_strip::StripView& strip, uint8_t candidate) {
    if (ctx.reel != ReelId::Left) return true;
    if (ctx.role == RoleFlag::WeakCherry || ctx.role == RoleFlag::StrongCherry) return true;
    return !visibleHas(strip, candidate, Symbol::Cherry);
}

} // namespace slotv2::stop_rules
