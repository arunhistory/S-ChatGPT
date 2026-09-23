#pragma once
#include <stdint.h>
#include "shared/rng.hpp"
#include "shared/types.hpp"

namespace slotv2::chain_zone {

static constexpr uint8_t kGamesPerSet = 5u;
struct State {
    bool active{false};
    uint8_t games_left{0u};
    uint32_t continuations{0u};
    uint32_t bonus_remaining{0u};
    uint8_t batch_size{0u};
    uint8_t batch_remaining{0u};
    uint8_t episode_position{0u};
};
struct Step {
    bool played{false};
    bool continued{false};
    bool ended{false};
    uint32_t earned_bonuses{0u};
};
struct Release {
    bool queued{false};
    bool episode{false};
    bool multi{false};
    uint8_t batch_size{0u};
};
void start(State& state);
bool continuationFromRoll(RoleFlag role, uint8_t roll_0_to_8);
Step playOne(State& state, Rng& rng, RoleFlag role);
uint16_t episodeThresholdPerThousand(uint8_t batch_size);
Release releaseNext(State& state, Rng& rng);

} // namespace slotv2::chain_zone
