#include <iostream>
#include "runtime/index.hpp"

int main() {
    bool ok = true;
    bool exercised = false;

    for (uint64_t seed = 1; seed < 100000 && !exercised; ++seed) {
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

        if (special != static_cast<uint32_t>(slotv2::SpecialHit::None)) {
            continue;
        }

        exercised = true;

        // 特化5Gの1Gを消費するが、通常ATの100Gは消費しない。
        ok = ok && state.machine.special_zone.games_left == 4u;
        ok = ok && state.machine.at.games_left == 100;
        ok = ok && !state.at_cycle.active;

        const auto result = state.special_zone_result;
        ok = ok && (
            result == slotv2::special_zone::HitResult::None
            || result == slotv2::special_zone::HitResult::AddGames
            || result == slotv2::special_zone::HitResult::Bonus
        );

        if (result == slotv2::special_zone::HitResult::AddGames) {
            ok = ok && slotv2::pending_event::has(
                state.pending,
                slotv2::pending_event::SpecialZoneAddGames
            );
        }

        if (result == slotv2::special_zone::HitResult::Bonus) {
            ok = ok && slotv2::pending_event::has(
                state.pending,
                slotv2::pending_event::SpecialZoneBonus
            );
        }
    }

    ok = ok && exercised;

    if (!ok) {
        std::cerr << "slot_v2_runtime_special_zone_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_runtime_special_zone_test: OK\n";
    return 0;
}
