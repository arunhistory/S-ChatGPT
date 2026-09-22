#pragma once
#include <stdint.h>
#include "../shared/rng.hpp"
#include "../shared/types.hpp"
#include "../session/index.hpp"
#include "../acquisition/index.hpp"
#include "../machine-state/index.hpp"
#include "../accounting/index.hpp"
#include "../point-ledger/index.hpp"
#include "../special-apply/index.hpp"
#include "../pending-event/index.hpp"
#include "../at-cycle/index.hpp"
#include "../at-resolution/index.hpp"
#include "../normal-mode/index.hpp"
#include "../progress-event/index.hpp"
#include "../cz-cycle/index.hpp"

namespace slotv2::runtime {

struct State {
    Rng rng{};
    session::State session{};
    acquisition::Result acquisition{};
    machine_state::State machine{};
    accounting::State accounting{};
    point_ledger::State points{};
    bool special_committed{false};
    special_apply::Result last_special_apply{};
    pending_event::State pending{};
    at_cycle::Result at_cycle{};
    at_resolution::Result at_resolution{};
    normal_mode::Mode normal_mode{normal_mode::Mode::NormalA};
    cz_cycle::Result cz_cycle{};
    bool at_hit_stock_gained{false};
};

void reset(State& state, uint64_t seed);
uint32_t lever(State& state);
uint32_t stop(State& state, uint32_t reel, uint32_t pressed_position);

// C++内部オーケストレーション用。TSからは呼ばせない。
bool recordCZResult(State& state, bool hit);
void recordNormalHit(State& state, bool was_at);
bool consumeNextHitAT(State& state);

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
uint32_t atActive(const State& state);
uint32_t atTier(const State& state);
int32_t atGamesLeft(const State& state);
uint32_t specialCommitted(const State& state);
uint32_t bellNavigationPacked(const State& state);
uint32_t bellNavigationNext(const State& state);
uint32_t bellNavigationCorrect(const State& state);
uint32_t pendingEvents(const State& state);
uint32_t atCyclePacked(const State& state);
uint32_t atResolutionPacked(const State& state);
uint32_t normalMode(const State& state);
uint32_t normalActualGames(const State& state);
uint32_t normalDisplayGames(const State& state);
uint32_t czCyclePacked(const State& state);

} // namespace slotv2::runtime
