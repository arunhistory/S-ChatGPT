#pragma once
#include <stdint.h>
#include "../shared/rng.hpp"

namespace slotv2::special_zone {

static constexpr uint8_t kGames = 5;

enum class HitResult : uint8_t {
    None = 0,
    AddGames = 1,
    Bonus = 2
};

struct State {
    bool active{false};
    uint8_t games_left{0};
};

void start(State& state);
HitResult playOne(State& state, Rng& rng);

} // namespace slotv2::special_zone
