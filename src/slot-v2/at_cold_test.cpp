#include <iostream>
#include "at-cold/index.hpp"

int main() {
    bool ok = true;

    uint32_t cold = 0u;
    for (uint8_t i = 0u; i < 10u; ++i) {
        cold += slotv2::at_cold::fromRoll(i) ? 1u : 0u;
    }

    ok = ok && cold == 6u;
    ok = ok && slotv2::at_cold::scaleGrowthCount(1000u, false) == 1000u;
    ok = ok && slotv2::at_cold::scaleGrowthCount(1000u, true) == 700u;
    ok = ok && slotv2::at_cold::scaleGrowthCount(333u, true) == 233u;

    if (!ok) {
        std::cerr << "slot_v2_at_cold_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_at_cold_test: OK\n";
    return 0;
}
