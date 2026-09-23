#include <iostream>
#include "normal-state/index.hpp"
#include "cz-cycle/index.hpp"
#include "cz-lottery/index.hpp"

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

    ok = ok && !slotv2::cz_lottery::fromRoll(slotv2::RoleFlag::OneMedal, 0u);
    ok = ok && slotv2::cz_lottery::fromRoll(slotv2::RoleFlag::StrongCherry, 999u);
    ok = ok && slotv2::cz_lottery::fromRoll(slotv2::RoleFlag::PenguinChance, 999u);
    ok = ok && slotv2::cz_lottery::fromRoll(slotv2::RoleFlag::Bell9, 219u);
    ok = ok && !slotv2::cz_lottery::fromRoll(slotv2::RoleFlag::Bell9, 220u);

    {
        slotv2::Rng rng(0xC2C2C2ULL);

        for (int trial = 0; trial < 1000; ++trial) {
            slotv2::cz_state::State s{};
            slotv2::cz_state::start(s);

            uint32_t played = 0u;
            bool ended = false;

            while (s.active) {
                // One-medal never breaks through, so this path must consume all 10G.
                const auto r = slotv2::cz_cycle::playOne(
                    rng,
                    s,
                    slotv2::RoleFlag::OneMedal
                );
                ok = ok && r.active;
                ++played;

                if (r.ended) {
                    ended = true;
                    break;
                }
            }

            ok = ok && played == 10u;
            ok = ok && ended;
            ok = ok && !s.active;
        }
    }

    if (!ok) {
        std::cerr << "slot_v2_normal_cz_cycle_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_normal_cz_cycle_test: OK\n";
    return 0;
}
