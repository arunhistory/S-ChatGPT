#include <iostream>
#include "at/upper_comeback_cycle.hpp"

int main() {
    bool ok = true;
    slotv2::Rng rng(0x64C0BACCULL);

    for (int trial = 0; trial < 1000; ++trial) {
        slotv2::upper_comeback::State state{};
        slotv2::upper_comeback::start(state);

        uint32_t played = 0u;
        bool judged = false;

        while (state.active) {
            const auto r = slotv2::upper_comeback_cycle::playOne(
                rng,
                state
            );

            ok = ok && r.active_before;
            ++played;

            if (r.ended) judged = true;
        }

        ok = ok && played == 64u;
        ok = ok && judged;
        ok = ok && state.games_left == 0u;
    }

    if (!ok) {
        std::cerr << "slot_v2_upper_comeback_cycle_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_upper_comeback_cycle_test: OK\n";
    return 0;
}
