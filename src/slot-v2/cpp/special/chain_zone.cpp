#include "special/chain_zone.hpp"

namespace slotv2::chain_zone {

void start(State& state) {
    state = {};
    state.active=true;
    state.games_left=kGamesPerSet;
}

Step playOne(State& state, bool combined_bell_replay_signal) {
    if (!state.active || state.games_left==0u) return {};
    Step out{};
    out.active_before=true;
    if (combined_bell_replay_signal && !state.signalled_this_set) {
        state.signalled_this_set=true;
        out.accepted_signal=true;
    }
    --state.games_left;
    if (state.games_left>0u) return out;

    if (state.signalled_this_set) {
        ++state.successful_continuations;
        if (state.successful_continuations % kContinuationsPerHit==0u
            && state.earned_hits<kMaxSimultaneousHits) {
            ++state.earned_hits;
        }
        // Release no later than four simultaneous hits. Do not keep
        // extending while silently throwing away the extra reward.
        if (state.earned_hits<kMaxSimultaneousHits) {
            state.games_left=kGamesPerSet;
            state.signalled_this_set=false;
            out.set_continued=true;
            return out;
        }
    }

    state.active=false;
    state.games_left=0u;
    out.finished=true;
    out.hits_ready=state.earned_hits;
    return out;
}

uint16_t simultaneousUpgradePerThousand(uint8_t hits) {
    switch (hits) {
        case 2u: return 50u;  // 5%
        case 3u: return 75u;  // 7.5%
        case 4u: return 100u; // 10%
        default: return 0u;  // single-hit baseline belongs elsewhere
    }
}

} // namespace slotv2::chain_zone
