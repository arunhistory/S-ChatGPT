#pragma once
#include <stdint.h>

// The new five-game AT chain zone, NOT the existing special_zone.
// The caller supplies the already-resolved, combined bell/replay
// continuation signal (target: 1/10 across eligible zone games).
// Keeping the role draw external avoids silently inventing a bell-vs-replay
// breakdown. Bonus payouts and episode upgrades are handled by the bonus
// pipeline AFTER this zone ends; this module returns the earned hit count.
namespace slotv2::chain_zone {

static constexpr uint8_t kGamesPerSet=5u;
static constexpr uint8_t kContinuationsPerHit=2u;
static constexpr uint8_t kMaxSimultaneousHits=4u;

struct State {
    bool active{false};
    bool signalled_this_set{false};
    uint8_t games_left{0u};
    uint8_t successful_continuations{0u};
    uint8_t earned_hits{0u};
};

struct Step {
    bool active_before{false};
    bool accepted_signal{false};
    bool set_continued{false};
    bool finished{false};
    uint8_t hits_ready{0u}; // nonzero only on the final step
};

void start(State& state);
Step playOne(State& state, bool combined_bell_replay_signal);

// Extra simultaneous-hit upgrade only: preserve the separate,
 // pre-existing one-bonus episode promotion logic outside this module.
uint16_t simultaneousUpgradePerThousand(uint8_t hits);

} // namespace slotv2::chain_zone
