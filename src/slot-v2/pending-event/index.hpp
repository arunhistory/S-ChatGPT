#pragma once
#include <stdint.h>

namespace slotv2::pending_event {

enum Bits : uint32_t {
    None             = 0u,
    BellFiveAT       = 1u << 0,
    CZThreeMissHit   = 1u << 1,
    NextHitAT        = 1u << 2
};

struct State {
    uint32_t bits{0};
};

void add(State& state, Bits event);
bool has(const State& state, Bits event);
bool consume(State& state, Bits event);
void clear(State& state);

} // namespace slotv2::pending_event
