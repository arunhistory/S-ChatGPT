#pragma once
#include <stdint.h>

namespace slotv2::bonus_state {

enum class Kind : uint8_t {
    Regular = 0,
    Episode = 1
};

static constexpr int kRegularTargetMedals = 50;
static constexpr int kEpisodeTargetMedals = 80;

struct State {
    bool active{false};
    Kind kind{Kind::Regular};
    int medals_left{0};
    bool suppress_regular_upgrade{false};
};

void start(State& state, Kind kind);
int applyNetGain(State& state, int medals);
bool complete(const State& state);

} // namespace slotv2::bonus_state
