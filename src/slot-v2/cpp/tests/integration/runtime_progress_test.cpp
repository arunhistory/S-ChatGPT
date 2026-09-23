#include <iostream>
#include "runtime/index.hpp"

int main() {
    bool ok = true;

    slotv2::runtime::State state{};
    slotv2::runtime::reset(state, 0x1234ULL);

    ok = ok && !slotv2::runtime::recordCZResult(state, false);
    ok = ok && !slotv2::runtime::recordCZResult(state, false);
    ok = ok && slotv2::runtime::recordCZResult(state, false);

    ok = ok && (
        slotv2::runtime::pendingEvents(state)
        & slotv2::pending_event::CZThreeMissHit
    ) != 0u;

    for (int i = 0; i < 5; ++i) {
        slotv2::runtime::recordNormalHit(state, false);
    }

    ok = ok && (
        slotv2::runtime::pendingEvents(state)
        & slotv2::pending_event::NextHitAT
    ) != 0u;

    ok = ok && slotv2::runtime::consumeNextHitAT(state);
    ok = ok && (
        slotv2::runtime::pendingEvents(state)
        & slotv2::pending_event::NextHitAT
    ) == 0u;

    if (!ok) {
        std::cerr << "slot_v2_runtime_progress_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_runtime_progress_test: OK\n";
    return 0;
}
