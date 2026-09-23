#include <iostream>
#include "shared/rng.hpp"
#include "at/at_lottery.hpp"

int main() {
    bool ok = true;
    slotv2::Rng rng(0xABCDEFULL);

    const slotv2::at_state::Table tables[] = {
        slotv2::at_state::Table::Normal,
        slotv2::at_state::Table::Heaven,
        slotv2::at_state::Table::SuperHeaven,
        slotv2::at_state::Table::Specialized,
    };
    const slotv2::at_state::Tier tiers[] = {
        slotv2::at_state::Tier::Lower,
        slotv2::at_state::Tier::Middle,
        slotv2::at_state::Tier::Upper,
    };

    uint64_t hit=0, fall=0, add=0, special=0, episode=0, upper=0, chain=0;

    for (const auto tier : tiers) {
        for (const auto table : tables) {
            slotv2::at_state::State state{};
            state.active = true;
            state.tier = tier;
            state.table = table;
            state.games_left = 100;

            for (uint64_t i = 0; i < 1000000ULL; ++i) {
                const auto d = slotv2::at_lottery::draw(rng, state);
                ok = ok && slotv2::at_lottery::count(d) <= 1u;
                hit += d.hit;
                fall += d.fall;
                add += d.add_games;
                special += d.special;
                episode += d.episode;
                upper += d.upper_special;
                chain += d.chain_zone;
            }
        }
    }

    ok = ok && hit > 0 && fall > 0 && add > 0
        && special > 0 && episode > 0 && upper > 0 && chain > 0;

    if (!ok) {
        std::cerr << "slot_v2_at_lottery_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_at_lottery_test: OK"
              << " hit=" << hit
              << " fall=" << fall
              << " add=" << add
              << " special=" << special
              << " episode=" << episode
              << " upper=" << upper
              << " chain=" << chain
              << "\n";
    return 0;
}
