#include <iostream>
#include <stdint.h>
#include "shared/rng.hpp"
#include "stock/stock_count_lottery.hpp"

int main() {
    bool ok = true;
    using slotv2::stock_count_lottery::Profile;
    using slotv2::stock_count_lottery::fromRoll;

    uint32_t middle[6]{0,0,0,0,0,0};
    uint32_t upper[6]{0,0,0,0,0,0};

    for (uint16_t roll = 0; roll < 1000; ++roll) {
        ++middle[fromRoll(roll, Profile::Middle)];
        ++upper[fromRoll(roll, Profile::Upper)];
    }

    ok = ok
        && middle[1] == 800
        && middle[2] == 150
        && middle[3] == 40
        && middle[4] == 9
        && middle[5] == 1;

    ok = ok
        && upper[1] == 600
        && upper[2] == 250
        && upper[3] == 100
        && upper[4] == 45
        && upper[5] == 5;

    slotv2::Rng rng(0xCAFEBABEULL);
    for (int i = 0; i < 10000; ++i) {
        const auto m = slotv2::stock_count_lottery::draw(rng, Profile::Middle);
        const auto u = slotv2::stock_count_lottery::draw(rng, Profile::Upper);
        ok = ok && m >= 1u && m <= 5u;
        ok = ok && u >= 1u && u <= 5u;
    }

    ok = ok && slotv2::stock_count_lottery::draw(rng, Profile::None) == 0u;

    if (!ok) {
        std::cerr << "slot_v2_stock_count_lottery_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_stock_count_lottery_test: OK\n";
    return 0;
}
