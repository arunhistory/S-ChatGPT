#include <iostream>
#include "normal-state/index.hpp"
#include "cz-cycle/index.hpp"

int main() {
    bool ok = true;

    {
        slotv2::normal_state::State s{};
        slotv2::normal_state::onLever(s);
        slotv2::normal_state::onLever(s);
        slotv2::normal_state::addDisplayGames(s, 50u);

        ok = ok && s.actual_games == 2u;
        ok = ok && s.display_games == 52u;

        slotv2::normal_state::reset(s);
        ok = ok && s.actual_games == 0u && s.display_games == 0u;
    }

    {
        slotv2::Rng rng(0xC2C2C2ULL);
        slotv2::cz_state::State s{};
        slotv2::cz_state::start(s);

        uint32_t played = 0u;
        while (s.active) {
            const auto r = slotv2::cz_cycle::playOne(rng, s);
            ok = ok && r.active;
            ++played;
        }

        ok = ok && played == 10u;
        ok = ok && s.games_left == 0u;
    }

    if (!ok) {
        std::cerr << "slot_v2_normal_cz_cycle_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_normal_cz_cycle_test: OK\n";
    return 0;
}
