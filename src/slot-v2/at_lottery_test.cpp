#include <iostream>
#include "shared/rng.hpp"
#include "at-lottery/index.hpp"

int main() {
    bool ok = true;

    // oneIn(1)は必ず成立、0は必ず不成立。
    slotv2::Rng rng(0xABCDEFULL);
    ok = ok && rng.oneIn(1u);
    ok = ok && !rng.oneIn(0u);

    // 通常のAT抽選を多数回通して、全フラグ経路が実際に到達可能か確認。
    uint64_t hit=0, fall=0, add=0, special=0, episode=0, upper=0;
    for (uint64_t i = 0; i < 5000000ULL; ++i) {
        const auto d = slotv2::at_lottery::drawBase(rng);
        hit += d.hit;
        fall += d.fall;
        add += d.add_games;
        special += d.special;
        episode += d.episode;
        upper += d.upper_special;
    }

    ok = ok && hit > 0 && fall > 0 && add > 0
        && special > 0 && episode > 0 && upper > 0;

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
              << "\n";
    return 0;
}
