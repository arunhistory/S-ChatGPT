#include "reel/stop/bell_navigation.hpp"

namespace slotv2::bell_navigation {

Plan make(Rng& rng, RoleFlag role, bool navigation_enabled) {
    Plan out{};

    if (!navigation_enabled) return out;
    if (role != RoleFlag::Bell9 && role != RoleFlag::Bell15) return out;

    out.active = true;
    out.order_index = navigation::normalize(static_cast<uint32_t>(rng.next64()));
    out.order = navigation::fromIndex(out.order_index);
    return out;
}

} // namespace slotv2::bell_navigation
