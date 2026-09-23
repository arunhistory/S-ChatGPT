#include "core/pending_event.hpp"

namespace slotv2::pending_event {

void add(State& state, Bits event) {
    state.bits |= static_cast<uint32_t>(event);
}

bool has(const State& state, Bits event) {
    return (state.bits & static_cast<uint32_t>(event)) != 0u;
}

bool consume(State& state, Bits event) {
    const uint32_t bit = static_cast<uint32_t>(event);
    if ((state.bits & bit) == 0u) return false;
    state.bits &= ~bit;
    return true;
}

void clear(State& state) {
    state.bits = 0u;
}

} // namespace slotv2::pending_event
