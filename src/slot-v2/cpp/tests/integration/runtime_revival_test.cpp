#include <iostream>
#include "core/runtime.hpp"

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

    // The five-game revival also runs the ordinary Normal "hit" frame
    // independently from role-based revival. CZ/direct-AT/special-direct are
    // excluded. Search deterministic seeds until the hidden 1/1700 frame hits.
    bool saw_hidden_hit = false;
    for (uint64_t seed = 1; seed <= 100000 && !saw_hidden_hit; ++seed) {
        slotv2::runtime::State probe{};
        slotv2::runtime::reset(probe, seed);
        slotv2::runtime::startRevivalChallenge(
            probe,
            slotv2::at_state::Tier::Lower
        );
        probe.session.phase = slotv2::session::Phase::Complete;

        const uint32_t packed = slotv2::runtime::lever(probe);
        ok = ok && ((packed >> 8) & 0xffu) == 0u; // no special direct
        if (probe.machine.revival.kicked_normal_hit) {
            saw_hidden_hit = true;
            ok = ok && probe.machine.area == slotv2::machine_state::Area::Revival;
            ok = ok && !probe.machine.at.active;
            ok = ok && !probe.machine.cz.active;
        }
    }
    ok = ok && saw_hidden_hit;

    if (!ok) {
        std::cerr << "slot_v2_runtime_revival_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_runtime_revival_test: OK\n";
    return 0;
}
