#include "core/point_ledger.hpp"

namespace slotv2::point_ledger {

void add(State& state, int64_t amount) {
    state.points += amount;
}

bool consume(State& state, int64_t amount) {
    if (amount <= 0) return true;
    if (state.points < amount) return false;
    state.points -= amount;
    return true;
}

} // namespace slotv2::point_ledger
