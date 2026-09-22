#include <iostream>
#include "bonus-cycle/index.hpp"

int main() {
    bool ok = true;
    slotv2::Rng rng(0xB0B0B0ULL);

    {
        slotv2::bonus_state::State state{};
        slotv2::bonus_state::start(
            state,
            slotv2::bonus_state::Kind::Regular
        );

        auto r = slotv2::bonus_cycle::applyNetGain(
            rng,
            state,
            49
        );

        ok = ok && r.active_before;
        ok = ok && r.medals_before == 50;
        ok = ok && r.medals_after == 1;
        ok = ok && r.outcome == slotv2::bonus_cycle::Outcome::None;
        ok = ok && state.active;

        r = slotv2::bonus_cycle::applyNetGain(
            rng,
            state,
            1
        );

        ok = ok && r.medals_after == 0;
        ok = ok && !state.active;
        ok = ok && (
            r.outcome == slotv2::bonus_cycle::Outcome::Completed
            || r.outcome == slotv2::bonus_cycle::Outcome::EpisodeUpgradePending
        );
    }

    {
        slotv2::bonus_state::State state{};
        slotv2::bonus_state::start(
            state,
            slotv2::bonus_state::Kind::Episode
        );

        const auto r = slotv2::bonus_cycle::applyNetGain(
            rng,
            state,
            80
        );

        ok = ok && r.medals_before == 80;
        ok = ok && r.medals_after == 0;
        ok = ok && r.outcome == slotv2::bonus_cycle::Outcome::Completed;
        ok = ok && !state.active;
    }

    if (!ok) {
        std::cerr << "slot_v2_bonus_cycle_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_bonus_cycle_test: OK\n";
    return 0;
}
