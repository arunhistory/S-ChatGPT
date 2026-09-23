#include <iostream>
#include "special/upper_special.hpp"

int main() {
    bool ok = true;

    uint32_t total = 0u;
    for (uint16_t i = 0; i < 100u; ++i) {
        total += slotv2::upper_special::addGamesFromRoll(i);
    }

    // 18.55G per successful continuation.
    ok = ok && total == 1855u;

    slotv2::Rng rng(0x9000A11ULL);
    uint64_t added = 0u;
    const uint32_t trials = 200000u;

    for (uint32_t i = 0; i < trials; ++i) {
        slotv2::upper_special::State s{};
        slotv2::upper_special::start(s);

        while (s.active) {
            (void)slotv2::upper_special::playOne(s, rng);
        }
        added += s.total_added_games;
    }

    const double average = static_cast<double>(added) / trials;
    ok = ok && average > 160.0 && average < 174.0;

    if (!ok) {
        std::cerr << "slot_v2_upper_special_test: FAILED avg="
                  << average << "\n";
        return 1;
    }

    std::cout << "slot_v2_upper_special_test: OK avg="
              << average << "\n";
    return 0;
}
