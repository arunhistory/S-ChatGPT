#include <iostream>
#include "core/progress_event.hpp"

int main() {
    bool ok = true;

    slotv2::normal_progress::State progress{};
    slotv2::pending_event::State pending{};

    ok = ok && !slotv2::progress_event::onCZResolved(progress, pending, false);
    ok = ok && !slotv2::progress_event::onCZResolved(progress, pending, false);
    ok = ok && slotv2::progress_event::onCZResolved(progress, pending, false);
    ok = ok && slotv2::pending_event::has(
        pending,
        slotv2::pending_event::CZThreeMissHit
    );

    for (int i = 0; i < 5; ++i) {
        slotv2::progress_event::onNormalHitResolved(
            progress,
            pending,
            false
        );
    }

    ok = ok && slotv2::pending_event::has(
        pending,
        slotv2::pending_event::NextHitAT
    );

    ok = ok && slotv2::progress_event::consumeNextHitAT(
        progress,
        pending
    );

    ok = ok && !slotv2::pending_event::has(
        pending,
        slotv2::pending_event::NextHitAT
    );

    if (!ok) {
        std::cerr << "slot_v2_progress_event_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_progress_event_test: OK\n";
    return 0;
}
