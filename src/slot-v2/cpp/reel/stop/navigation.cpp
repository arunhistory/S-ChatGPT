#include "reel/stop/navigation.hpp"

namespace slotv2::navigation {

Order fromIndex(uint8_t index) {
    switch (index % kOrderCount) {
        case 0: return {{ReelId::Left,   ReelId::Middle, ReelId::Right}};
        case 1: return {{ReelId::Left,   ReelId::Right,  ReelId::Middle}};
        case 2: return {{ReelId::Middle, ReelId::Left,   ReelId::Right}};
        case 3: return {{ReelId::Middle, ReelId::Right,  ReelId::Left}};
        case 4: return {{ReelId::Right,  ReelId::Left,   ReelId::Middle}};
        default:return {{ReelId::Right,  ReelId::Middle, ReelId::Left}};
    }
}

uint8_t normalize(uint32_t value) {
    return static_cast<uint8_t>(value % kOrderCount);
}

bool matchesPrefix(const Order& order, const ReelId* stopped, uint8_t stopped_count) {
    if (stopped_count > 3u) return false;
    for (uint8_t i = 0; i < stopped_count; ++i) {
        if (order.reel[i] != stopped[i]) return false;
    }
    return true;
}

bool nextIs(const Order& order, const ReelId* stopped, uint8_t stopped_count, ReelId next) {
    if (stopped_count >= 3u) return false;
    if (!matchesPrefix(order, stopped, stopped_count)) return false;
    return order.reel[stopped_count] == next;
}

} // namespace slotv2::navigation
