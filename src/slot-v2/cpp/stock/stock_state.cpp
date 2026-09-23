#include "stock/stock_state.hpp"

namespace slotv2::stock {

void add(State& state, uint32_t amount) {
    state.count += amount;
}

bool consumeOne(State& state) {
    if (state.count == 0u) return false;
    --state.count;
    return true;
}

uint8_t preferenceLevel(const State& state) {
    if (state.count >= 5u) return 3u;
    if (state.count >= 3u) return 2u;
    if (state.count >= 1u) return 1u;
    return 0u;
}

uint8_t consumeForSection(State& state) {
    const uint8_t level = preferenceLevel(state);
    state.count = 0u;
    return level;
}

} // namespace slotv2::stock
