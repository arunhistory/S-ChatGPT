#include "bonus/bonus_cycle.hpp"
#include "bonus/bonus_upgrade.hpp"

namespace slotv2::bonus_cycle {

Result applyNetGain(
    Rng& rng,
    bonus_state::State& state,
    int net_gain
) {
    if (!state.active) return {};

    Result out{};
    out.active_before = true;
    out.kind = state.kind;
    out.medals_before = state.medals_left;

    bonus_state::applyNetGain(state, net_gain);
    out.medals_after = state.medals_left;

    if (!bonus_state::complete(state)) {
        return out;
    }

    if (out.kind == bonus_state::Kind::Regular
        && bonus_upgrade::toEpisode(rng)) {
        out.outcome = Outcome::EpisodeUpgradePending;
        return out;
    }

    out.outcome = Outcome::Completed;
    return out;
}

} // namespace slotv2::bonus_cycle
