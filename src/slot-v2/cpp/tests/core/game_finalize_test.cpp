#include <iostream>
#include "game-finalize/index.hpp"

int main() {
    bool ok = true;

    slotv2::normal_progress::State progress{};
    slotv2::pending_event::State pending{};

    slotv2::acquisition::Result acquired{
        slotv2::RoleFlag::Bell9,
        slotv2::acquisition::Status::Acquired,
        9
    };

    for (int i = 0; i < 4; ++i) {
        const auto r = slotv2::game_finalize::apply(
            slotv2::RoleFlag::Bell9,
            acquired,
            progress,
            pending
        );
        ok = ok && !r.bell5_at_triggered;
    }

    const auto fifth = slotv2::game_finalize::apply(
        slotv2::RoleFlag::Bell9,
        acquired,
        progress,
        pending
    );

    ok = ok && fifth.bell5_at_triggered;
    ok = ok && slotv2::pending_event::has(
        pending,
        slotv2::pending_event::BellFiveAT
    );

    // 別役を挟むと9枚ベル連続数はリセット。
    slotv2::game_finalize::apply(
        slotv2::RoleFlag::Replay,
        {slotv2::RoleFlag::Replay, slotv2::acquisition::Status::Acquired, 0},
        progress,
        pending
    );
    ok = ok && progress.bell9_streak == 0u;

    ok = ok && slotv2::pending_event::consume(
        pending,
        slotv2::pending_event::BellFiveAT
    );
    ok = ok && !slotv2::pending_event::has(
        pending,
        slotv2::pending_event::BellFiveAT
    );

    if (!ok) {
        std::cerr << "slot_v2_game_finalize_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_game_finalize_test: OK\n";
    return 0;
}
