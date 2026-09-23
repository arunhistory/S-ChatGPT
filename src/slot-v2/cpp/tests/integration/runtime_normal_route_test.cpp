#include <iostream>
#include "core/runtime.hpp"

namespace {

void stopAll(slotv2::runtime::State& state) {
    (void)slotv2::runtime::stop(state, 0u, 0u);
    (void)slotv2::runtime::stop(state, 1u, 0u);
    (void)slotv2::runtime::stop(state, 2u, 0u);
}

bool run777(uint64_t seed) {
    slotv2::runtime::State state{};
    slotv2::runtime::reset(state, seed);

    state.normal_mode = slotv2::normal_mode::Mode::Special;
    state.normal_route.pattern = 0u;
    state.normal_route.ceiling = 777u;
    state.normal_route.special_window_checked = true;
    state.normal_route.ceiling_consumed = false;
    state.machine.normal.actual_games = 776u;
    state.machine.normal.display_games = 776u;

    const uint32_t lever = slotv2::runtime::lever(state);
    const auto special = static_cast<slotv2::SpecialHit>((lever >> 8) & 0xffu);
    if (special != slotv2::SpecialHit::None) return false;

    stopAll(state);

    return state.normal_ceiling_transition.outcome
            == slotv2::normal_ceiling_transition::Outcome::ATWithStockQueued
        && state.normal_route.ceiling_consumed
        && state.machine.entry_gate.active
        && state.machine.entry_gate.kind == slotv2::entry_gate::Kind::AT
        && state.machine.entry_gate.at_tier == slotv2::at_state::Tier::Lower
        && state.machine.entry_gate.stock_to_add == 1u
        && !state.machine.at.active;
}

bool run1500(uint64_t seed) {
    slotv2::runtime::State state{};
    slotv2::runtime::reset(state, seed);

    state.normal_mode = slotv2::normal_mode::Mode::Special;
    state.normal_route.pattern = 0u;
    state.normal_route.ceiling = 1500u;
    state.normal_route.special_window_checked = true;
    state.normal_route.ceiling_consumed = false;
    state.machine.normal.actual_games = 1499u;
    state.machine.normal.display_games = 1499u;

    const uint32_t first = slotv2::runtime::lever(state);
    const auto first_special =
        static_cast<slotv2::SpecialHit>((first >> 8) & 0xffu);
    if (first_special != slotv2::SpecialHit::None) return false;

    stopAll(state);

    if (state.normal_ceiling_transition.outcome
            != slotv2::normal_ceiling_transition::Outcome::FreezeQueued
        || !state.normal_route.ceiling_consumed
        || !state.ceiling_freeze_pending) {
        return false;
    }

    const uint32_t before_games = state.machine.normal.actual_games;

    const uint32_t second = slotv2::runtime::lever(state);
    const auto second_special =
        static_cast<slotv2::SpecialHit>((second >> 8) & 0xffu);

    if (second_special != slotv2::SpecialHit::Freeze) return false;
    if (state.machine.normal.actual_games != before_games) return false;
    if (state.session.phase != slotv2::session::Phase::Stopping) return false;

    // Freeze ignores normal 0-4 slip and forces the exact BLUE777 positions.
    const auto l = slotv2::runtime::stop(state, 0u, 3u);
    const auto m = slotv2::runtime::stop(state, 1u, 7u);
    const auto r = slotv2::runtime::stop(state, 2u, 18u);

    if ((l & 0xffu) != 9u) return false;
    if ((m & 0xffu) != 14u) return false;
    if ((r & 0xffu) != 10u) return false;

    return state.machine.area == slotv2::machine_state::Area::AT
        && state.machine.at.active
        && state.machine.at.tier == slotv2::at_state::Tier::Upper
        && state.machine.stock.count >= 1u
        && state.special_committed
        && !state.ceiling_freeze_pending;
}

}

int main() {
    bool ok777 = false;
    bool ok1500 = false;

    // Skip extremely rare seeds where a direct special hit wins priority
    // on the same ceiling game.
    for (uint64_t seed = 1u; seed < 10000u && (!ok777 || !ok1500); ++seed) {
        if (!ok777) ok777 = run777(seed);
        if (!ok1500) ok1500 = run1500(seed + 0x10000u);
    }

    if (!ok777 || !ok1500) {
        std::cerr << "slot_v2_runtime_normal_route_test: FAILED"
                  << " g777=" << ok777
                  << " g1500=" << ok1500
                  << "\n";
        return 1;
    }

    std::cout << "slot_v2_runtime_normal_route_test: OK\n";
    return 0;
}
