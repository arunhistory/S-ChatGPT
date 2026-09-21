#pragma once
#include <stdint.h>
#include "../shared/types.hpp"

namespace slotv2::navigation {

struct Order {
    ReelId reel[3]{ReelId::Left, ReelId::Middle, ReelId::Right};
};

static constexpr uint8_t kOrderCount = 6;

// 0:L-M-R / 1:L-R-M / 2:M-L-R / 3:M-R-L / 4:R-L-M / 5:R-M-L
Order fromIndex(uint8_t index);
uint8_t normalize(uint32_t value);
bool matchesPrefix(const Order& order, const ReelId* stopped, uint8_t stopped_count);
bool nextIs(const Order& order, const ReelId* stopped, uint8_t stopped_count, ReelId next);

} // namespace slotv2::navigation
