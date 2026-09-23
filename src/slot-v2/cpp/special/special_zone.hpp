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
    uint16_t pending_add_games{0};
};

void start(State& state);

// Approved special-add distribution:
// +20 30%, +30 25%, +40 20%, +50 15%, +100 8%, +200 2%.
// Mean per add hit: 41G.
uint16_t addGamesFromRoll(uint16_t roll_0_to_99);

HitResult playOne(State& state, Rng& rng);

} // namespace slotv2::special_zone
