#include <iostream>
#include "cz/cz_reward.hpp"

int main() {
    bool ok = true;

    uint32_t normal = 0u;
    uint32_t at = 0u;
    for (uint16_t i = 0; i < 10u; ++i) {
        const auto t = slotv2::cz_reward::targetFromRoll(i);
        if (t == slotv2::cz_reward::Target::LowerAT) ++at;
        else ++normal;
    }

    ok = ok && normal == 7u && at == 3u;

    if (!ok) {
        std::cerr << "slot_v2_cz_reward_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_cz_reward_test: OK\n";
    return 0;
}
