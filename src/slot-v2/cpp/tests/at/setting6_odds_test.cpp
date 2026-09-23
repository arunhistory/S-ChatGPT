#include <array>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include "shared/types.hpp"
#include "core/setting_profile.hpp"
#include "at/at_setting6_plan.hpp"

int main() {
    using namespace slotv2;
    const auto raw = setting_profile::normalRaw(6u);
    const double n = static_cast<double>(kRngSpace);
    const double pAT = raw.at_threshold_27/n;
    const double pBonus = (1.0-pAT)*raw.bonus_threshold_27/n;
    const double pCZ = (1.0-pAT)*(1.0-raw.bonus_threshold_27/n)
                           *raw.cz_threshold_27/n;

    bool ok = std::abs(1.0/pAT - 2650.0) < 0.05
        && std::abs(1.0/pBonus - 1700.0) < 0.05
        && std::abs(1.0/pCZ - 1150.0) < 0.05
        && at_state::kUpperNetPerGame == 9;

    constexpr std::array tiers{
        at_state::Tier::Lower, at_state::Tier::Middle, at_state::Tier::Upper
    };
    constexpr std::array tables{
        at_state::Table::Normal, at_state::Table::Heaven,
        at_state::Table::SuperHeaven, at_state::Table::Specialized
    };
    for (auto tier : tiers) {
        for (auto table : tables) {
            const auto x = at_setting6_plan::forTierTable(tier, table);
            const auto totalDen = tier==at_state::Tier::Lower ? 80u : 50u;
            const auto chainDen = tier==at_state::Tier::Lower ? 500u
                : (tier==at_state::Tier::Middle ? 450u : 400u);
            ok = ok && x.positive()==at_setting6_plan::nearestCount(totalDen);
            ok = ok && x.chain_zone==at_setting6_plan::nearestCount(chainDen);
            ok = ok && x.fall==at_setting6_plan::nearestCount(400u);
            ok = ok && x.all()<kRngSpace;
            ok = ok && x.hit>0 && x.add_games>0 && x.special>0;
            ok = ok && (x.episode>0)==
                (table==at_state::Table::Heaven ||
                 table==at_state::Table::SuperHeaven);
            ok = ok && (x.upper_special>0)==
                (table==at_state::Table::Specialized);
        }
    }
    if (!ok) {
        std::cerr << "setting6_odds_test: FAIL\n";
        return EXIT_FAILURE;
    }
    std::cout << "setting6_odds_test: PASS"
              << " raw_AT=1/" << 1.0/pAT
              << " raw_bonus=1/" << 1.0/pBonus
              << " raw_CZ=1/" << 1.0/pCZ
              << " upper_net=" << at_state::kUpperNetPerGame
              << " and all 12 tier/table AT plans checked\n";
    return EXIT_SUCCESS;
}
