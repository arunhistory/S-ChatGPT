#pragma once
#include <stdint.h>
#include "../shared/types.hpp"
#include "../stop-shared/index.hpp"
#include "../freeze/index.hpp"
#include "../special-result/index.hpp"

namespace slotv2::session {

enum class Phase : uint8_t {
    Idle = 0,
    Stopping = 1,
    SpecialPending = 2,
    Complete = 3
};

struct State {
    Phase phase{Phase::Idle};
    LeverResult lever{};
    special_result::Result special{};
    freeze::Directive freeze{};
    bool stopped[3]{false,false,false};
    uint8_t position[3]{0,0,0};
    uint8_t stop_count{0};
};

void reset(State& state);
bool canLever(const State& state);
bool begin(
    State& state,
    LeverResult lever,
    special_result::Result special,
    freeze::Directive freeze
);
bool canStop(const State& state, ReelId reel);
stop_shared::Context makeStopContext(
    const State& state,
    ReelId reel,
    uint8_t pressed_position
);
void acceptStop(State& state, ReelId reel, const stop_shared::Result& result);
void completeSpecial(State& state);

} // namespace slotv2::session
