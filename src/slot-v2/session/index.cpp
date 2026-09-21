#include "index.hpp"

namespace slotv2::session {

void reset(State& state) {
    state = {};
}

void begin(State& state, LeverResult lever, freeze::Directive freeze) {
    state.lever = lever;
    state.freeze = freeze;
    state.stop_count = 0;
    for (int i = 0; i < 3; ++i) state.stopped[i] = false;
}

stop_shared::Context makeStopContext(
    const State& state,
    ReelId reel,
    uint8_t pressed_position
) {
    stop_shared::Context ctx{};
    ctx.role = state.lever.role;
    ctx.reel = reel;
    ctx.pressed_position = static_cast<uint8_t>(pressed_position % kReelSize);
    ctx.stop_order = state.stop_count;

    for (int i = 0; i < 3; ++i) {
        ctx.stopped[i] = state.stopped[i];
        ctx.stopped_position[i] = state.position[i];
    }
    return ctx;
}

void acceptStop(State& state, ReelId reel, const stop_shared::Result& result) {
    if (result.status != stop_shared::ResolveStatus::Ok
        && result.status != stop_shared::ResolveStatus::RoleMissed) {
        return;
    }

    const auto i = static_cast<uint8_t>(reel);
    if (i > 2u || state.stopped[i]) return;

    state.stopped[i] = true;
    state.position[i] = result.final_position;
    if (state.stop_count < 3u) ++state.stop_count;
}

} // namespace slotv2::session
