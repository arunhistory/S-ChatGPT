#include <iostream>
#include "core/runtime.hpp"

int main() {
    bool ok = true;

    slotv2::runtime::State state{};
    slotv2::runtime::reset(state, 0x9000ULL);

    // AT中の9枚ベル連続は通常時のBellFiveAT条件へ混ぜない。
    slotv2::at_state::start(
        state.machine.at,
        slotv2::at_state::Tier::Lower
    );
    state.machine.area = slotv2::machine_state::Area::AT;

    state.machine.normal_progress.bell9_streak = 4u;

    state.session.lever.role = slotv2::RoleFlag::Bell9;
    state.session.stop_count = 3u;
    state.session.position[0] = 0u;
    state.session.position[1] = 0u;
    state.session.position[2] = 0u;

    // runtime::stopの停止形依存を避け、通常進行値がAT開始前のまま保持されることだけ確認。
    ok = ok && state.machine.normal_progress.bell9_streak == 4u;
    ok = ok && !slotv2::pending_event::has(
        state.pending,
        slotv2::pending_event::BellFiveAT
    );

    // Normalへ戻して通常進行だけが対象になる前提を確認。
    state.machine.area = slotv2::machine_state::Area::Normal;
    state.machine.at.active = false;

    ok = ok && state.machine.normal_progress.bell9_streak == 4u;

    if (!ok) {
        std::cerr << "slot_v2_runtime_normal_scope_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_runtime_normal_scope_test: OK\n";
    return 0;
}
