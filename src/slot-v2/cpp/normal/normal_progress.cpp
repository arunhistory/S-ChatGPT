#include "index.hpp"

namespace slotv2::normal_progress {

bool onBell9(State& state) {
    if (state.bell9_streak < 5u) ++state.bell9_streak;
    if (state.bell9_streak >= 5u) {
        state.bell9_streak = 0u;
        return true;
    }
    return false;
}

void onNonBell9(State& state) {
    state.bell9_streak = 0u;
}

bool onCZResult(State& state, bool hit) {
    if (hit) {
        state.cz_misses = 0u;
        return false;
    }

    if (state.cz_misses < 3u) ++state.cz_misses;
    if (state.cz_misses >= 3u) {
        state.cz_misses = 0u;
        return true;
    }
    return false;
}

void onNormalHit(State& state, bool was_at) {
    if (was_at) {
        state.normal_hits_without_at = 0u;
        state.next_hit_at_guaranteed = false;
        return;
    }

    if (state.normal_hits_without_at < 5u) ++state.normal_hits_without_at;
    if (state.normal_hits_without_at >= 5u) {
        state.normal_hits_without_at = 0u;
        state.next_hit_at_guaranteed = true;
    }
}

bool consumeNextATGuarantee(State& state) {
    if (!state.next_hit_at_guaranteed) return false;
    state.next_hit_at_guaranteed = false;
    return true;
}

} // namespace slotv2::normal_progress
