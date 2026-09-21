#pragma once
#include <stdint.h>
#include "../shared/types.hpp"
#include "../stop-shared/index.hpp"
#include "../freeze/index.hpp"

namespace slotv2::session {

struct State {
    LeverResult lever{};
    freeze::Directive freeze{};
    bool stopped[3]{false,false,false};
    uint8_t position[3]{0,0,0};
    uint8_t stop_count{0};
};

void reset(State& state);
void begin(State& state, LeverResult lever, freeze::Directive freeze);
stop_shared::Context makeStopContext(
    const State& state,
    ReelId reel,
    uint8_t pressed_position
);
void acceptStop(State& state, ReelId reel, const stop_shared::Result& result);

} // namespace slotv2::session
