#include <iostream>
#include "bell-navigation/index.hpp"

int main() {
    bool ok = true;
    slotv2::Rng rng(0x123456789ULL);

    const auto disabled = slotv2::bell_navigation::make(
        rng,
        slotv2::RoleFlag::Bell9,
        false
    );
    ok = ok && !disabled.active;

    const auto replay = slotv2::bell_navigation::make(
        rng,
        slotv2::RoleFlag::Replay,
        true
    );
    ok = ok && !replay.active;

    bool firstSeen[3]{false,false,false};
    for (int i = 0; i < 120; ++i) {
        const auto plan = slotv2::bell_navigation::make(
            rng,
            slotv2::RoleFlag::Bell9,
            true
        );
        ok = ok && plan.active && plan.order_index < 6u;
        firstSeen[static_cast<uint8_t>(plan.order.reel[0])] = true;
    }

    ok = ok && firstSeen[0] && firstSeen[1] && firstSeen[2];

    if (!ok) {
        std::cerr << "slot_v2_bell_navigation_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_bell_navigation_test: OK\n";
    return 0;
}
