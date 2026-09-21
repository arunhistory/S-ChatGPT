#pragma once
#include <stdint.h>

namespace slotv2::point_ledger {

struct State {
    int64_t points{0};
};

void add(State& state, int64_t amount);
bool consume(State& state, int64_t amount);

} // namespace slotv2::point_ledger
