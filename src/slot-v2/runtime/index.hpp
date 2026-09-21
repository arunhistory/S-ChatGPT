#pragma once
#include <stdint.h>
#include "../shared/rng.hpp"
#include "../shared/types.hpp"
#include "../session/index.hpp"
#include "../acquisition/index.hpp"

namespace slotv2::runtime {

struct State {
    Rng rng{};
    session::State session{};
    acquisition::Result acquisition{};
};

void reset(State& state, uint64_t seed);
uint32_t lever(State& state);
uint32_t stop(State& state, uint32_t reel, uint32_t pressed_position);

uint32_t phase(const State& state);
uint32_t specialResult(const State& state);
uint32_t completeSpecial(State& state);
uint32_t stoppedPosition(const State& state, uint32_t reel);
uint32_t stopSequence(const State& state, uint32_t order_index);
uint32_t acquisitionPacked(const State& state);
uint32_t lastSpecial(const State& state);
uint32_t lastRole(const State& state);
uint32_t freezeActive(const State& state);

} // namespace slotv2::runtime
