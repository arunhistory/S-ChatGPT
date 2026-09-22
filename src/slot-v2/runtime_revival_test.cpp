#include <iostream>
#include "runtime/index.hpp"

int main() {
    bool ok = true;

    slotv2::runtime::State state{};
    slotv2::runtime::reset(state, 0x52564956ULL);

    slotv2::at_state::start(
        state.machine.at,
        slotv2::at_state::Tier::Upper
    );
    state.machine.area = slotv2::machine_state::Area::AT;
    state.session.phase = slotv2::session::Phase::Complete;

    slotv2::runtime::startRevivalChallenge(
        state,
        slotv2::at_state::Tier::Upper
    );

    ok = ok && state.machine.area == slotv2::machine_state::Area::Revival;
    ok = ok && state.machine.revival.active;
    ok = ok && state.machine.revival.games_left == 5u;
    ok = ok && !state.machine.at.active;

    slotv2::runtime::recordRevivalHiddenNormalHit(state);
    ok = ok && state.machine.revival.kicked_normal_hit;

    const uint32_t lever = slotv2::runtime::lever(state);

    // 復活チャレンジでは特殊直撃を抽選せず、通常小役の本抽選だけ走る。
    ok = ok && ((lever >> 8) & 0xffu) == 0u;
    ok = ok && ((lever >> 16) & 1u) == 1u;
    ok = ok && state.revival_game.active;
    ok = ok && state.revival_game.games_before == 5u;
    ok = ok && state.revival_game.games_after == 4u;

    if (!ok) {
        std::cerr << "slot_v2_runtime_revival_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_runtime_revival_test: OK\n";
    return 0;
}
