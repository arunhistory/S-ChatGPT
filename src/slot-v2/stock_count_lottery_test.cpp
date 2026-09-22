#include <iostream>
#include <stdint.h>
#include "shared/rng.hpp"
#include "stock-count-lottery/index.hpp"

int main() {
    bool ok = true;
    slotv2::Rng rng(0xCAFEBABEULL);

    uint64_t middle[6]{0,0,0,0,0,0};
    uint64_t upper[6]{0,0,0,0,0,0};

    for (uint64_t i = 0; i < 2000000ULL; ++i) {
        const auto m = slotv2::stock_count_lottery::draw(
            rng,
            slotv2::stock_count_lottery::Profile::Middle
        );
        const auto u = slotv2::stock_count_lottery::draw(
            rng,
            slotv2::stock_count_lottery::Profile::Upper
        );

        ok = ok && m >= 1u && m <= 5u;
        ok = ok && u >= 1u && u <= 5u;
        ++middle[m];
        ++upper[u];
    }

    ok = ok && slotv2::stock_count_lottery::draw(
        rng,
        slotv2::stock_count_lottery::Profile::None
    ) == 0u;

    for (int i = 1; i <= 5; ++i) {
        ok = ok && middle[i] > 0 && upper[i] > 0;
    }

    if (!ok) {
        std::cerr << "slot_v2_stock_count_lottery_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_stock_count_lottery_test: OK"
              << " middle="
              << middle[1] << "," << middle[2] << "," << middle[3] << "," << middle[4] << "," << middle[5]
              << " upper="
              << upper[1] << "," << upper[2] << "," << upper[3] << "," << upper[4] << "," << upper[5]
              << "\n";
    return 0;
}
