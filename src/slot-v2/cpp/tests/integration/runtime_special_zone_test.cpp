#include <iostream>
#include "core/runtime.hpp"

namespace {

void stopAll(slotv2::runtime::State& state) {
    (void)slotv2::runtime::stop(state, 0u, 0u);
    (void)slotv2::runtime::stop(state, 1u, 0u);
    (void)slotv2::runtime::stop(state, 2u, 0u);
}

}

int main() {
    bool ok = true;
    bool saw_add = false;
    bool saw_bonus = false;
    bool saw_none = false;

    for (uint64_t seed = 1; seed < 200000 && !(saw_add && saw_bonus && saw_none); ++seed) {
        slotv2::runtime::State state{};
        slotv2::runtime::reset(state, seed);

        slotv2::at_state::start(
            state.machine.at,
            slotv2::at_state::Tier::Lower
        );
        state.machine.area = slotv2::machine_state::Area::AT;
        slotv2::special_zone::start(state.machine.special_zone);

        const uint32_t lever = slotv2::runtime::lever(state);
        const uint32_t special = (lever >> 8) & 0xffu;
        ok = ok && special == static_cast<uint32_t>(slotv2::SpecialHit::None);

        // Special-zone game consumes its own 1G, never the base AT ST.
        ok = ok && state.machine.at.games_left == 100;
        ok = ok && !state.at_cycle.active;

        const auto result = state.special_zone_result;
        const uint16_t fixed_add = state.machine.special_zone.pending_add_games;

        if (result == slotv2::special_zone::HitResult::AddGames) {
            saw_add = true;
            ok = ok && fixed_add > 0u;
            ok = ok && slotv2::pending_event::has(
                state.pending,
                slotv2::pending_event::SpecialZoneAddGames
            );

            stopAll(state);

            ok = ok && state.special_zone_transition.outcome
                == slotv2::special_zone_transition::Outcome::AddGamesApplied;
            ok = ok && state.special_zone_transition.added_games == fixed_add;
            ok = ok && state.machine.at.games_left == 100 + fixed_add;
            ok = ok && !slotv2::pending_event::has(
                state.pending,
                slotv2::pending_event::SpecialZoneAddGames
            );
        } else if (result == slotv2::special_zone::HitResult::Bonus) {
            saw_bonus = true;
            ok = ok && !state.machine.special_zone.active;
            ok = ok && state.machine.special_zone.games_left == 0u;
            ok = ok && slotv2::pending_event::has(
                state.pending,
                slotv2::pending_event::SpecialZoneBonus
            );

            stopAll(state);

            ok = ok && state.special_zone_transition.outcome
                == slotv2::special_zone_transition::Outcome::BonusQueued;
            ok = ok && state.machine.entry_gate.active;
            ok = ok && state.machine.entry_gate.kind
                == slotv2::entry_gate::Kind::Bonus;
            ok = ok && state.machine.entry_gate.bonus_return_to_at;
            ok = ok && !state.machine.bonus.active;
            ok = ok && !slotv2::pending_event::has(
                state.pending,
                slotv2::pending_event::SpecialZoneBonus
            );
        } else {
            saw_none = true;
            stopAll(state);
            ok = ok && state.machine.at.games_left == 100;
            ok = ok && state.special_zone_transition.outcome
                == slotv2::special_zone_transition::Outcome::None;
        }
    }

    ok = ok && saw_add && saw_bonus && saw_none;

    if (!ok) {
        std::cerr << "slot_v2_runtime_special_zone_test: FAILED"
                  << " add=" << saw_add
                  << " bonus=" << saw_bonus
                  << " none=" << saw_none
                  << "\n";
        return 1;
    }

    std::cout << "slot_v2_runtime_special_zone_test: OK\n";
    return 0;
}
