#include "section/section_state.hpp"

namespace slotv2::section {

ApplyResult apply(State& state, int64_t medal_delta) {
    state.current_diff += medal_delta;
    if (state.current_diff < state.minimum_diff) {
        state.minimum_diff = state.current_diff;
    }

    const int64_t gain = state.current_diff - state.minimum_diff;
    if (gain < kCutGain) {
        return {
            false,
            state.current_diff,
            state.minimum_diff,
            gain,
            state.section_count
        };
    }

    const int64_t ended_diff = state.current_diff;
    const int64_t ended_min = state.minimum_diff;

    ++state.section_count;
    state.current_diff = 0;
    state.minimum_diff = 0;

    return {
        true,
        ended_diff,
        ended_min,
        gain,
        state.section_count
    };
}

} // namespace slotv2::section
