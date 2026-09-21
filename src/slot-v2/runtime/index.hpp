#pragma once
#include <stdint.h>
#include "../shared/rng.hpp"
#include "../shared/types.hpp"
#include "../session/index.hpp"
#include "../acquisition/index.hpp"
#include "../machine-state/index.hpp"
#include "../accounting/index.hpp"
#include "../point-ledger/index.hpp"

namespace slotv2::runtime {

struct State {
    Rng rng{};
    session::State session{};
    acquisition::Result acquisition{};
    machine_state::State machine{};
    accounting::State accounting{};
    point_ledger::State points{};
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

uint32_t machineArea(const State& state);
int64_t sectionDiff(const State& state);
int64_t sectionMinimum(const State& state);
uint64_t sectionCount(const State& state);
uint32_t stockCount(const State& state);
int64_t pointCount(const State& state);

} // namespace slotv2::runtime
