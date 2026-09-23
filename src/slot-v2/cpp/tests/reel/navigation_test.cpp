#include <iostream>
#include "reel/stop/navigation.hpp"

int main() {
    bool ok = true;
    bool firstSeen[3]{false,false,false};

    for (uint8_t i = 0; i < slotv2::navigation::kOrderCount; ++i) {
        const auto order = slotv2::navigation::fromIndex(i);

        bool seen[3]{false,false,false};
        for (int p = 0; p < 3; ++p) {
            const auto r = static_cast<uint8_t>(order.reel[p]);
            if (r > 2u || seen[r]) ok = false;
            seen[r] = true;
        }

        firstSeen[static_cast<uint8_t>(order.reel[0])] = true;

        slotv2::ReelId stopped[3]{};
        ok = ok && slotv2::navigation::nextIs(order, stopped, 0, order.reel[0]);
        stopped[0] = order.reel[0];
        ok = ok && slotv2::navigation::nextIs(order, stopped, 1, order.reel[1]);
        stopped[1] = order.reel[1];
        ok = ok && slotv2::navigation::nextIs(order, stopped, 2, order.reel[2]);
    }

    ok = ok && firstSeen[0] && firstSeen[1] && firstSeen[2];

    if (!ok) {
        std::cerr << "slot_v2_navigation_test: FAILED\n";
        return 1;
    }
    std::cout << "slot_v2_navigation_test: OK\n";
    return 0;
}
