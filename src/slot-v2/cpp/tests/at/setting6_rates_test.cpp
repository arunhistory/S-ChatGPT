#include <cmath>
#include <iostream>
#include "at/at_lottery.hpp"
#include "core/setting_profile.hpp"
#include "shared/types.hpp"

int main() {
    bool ok=true;
    const auto raw=slotv2::setting_profile::normalRaw(6u);
    const double n=static_cast<double>(slotv2::kRngSpace);
    const double at=raw.at_threshold_27/n;
    const double bonus=(1.0-at)*(raw.bonus_threshold_27/n);
    const double cz=(1.0-at)*(1.0-raw.bonus_threshold_27/n)
                    *(raw.cz_threshold_27/n);
    ok=ok && std::abs(at-1.0/2650.0)<1e-7;
    ok=ok && std::abs(bonus-1.0/1700.0)<1e-7;
    ok=ok && std::abs(cz-1.0/1150.0)<1e-7;

    const slotv2::at_state::Table tables[]={
        slotv2::at_state::Table::Normal,
        slotv2::at_state::Table::Heaven,
        slotv2::at_state::Table::SuperHeaven,
        slotv2::at_state::Table::Specialized
    };
    const double weights[]={0.4375,0.25,0.0625,0.25};
    const slotv2::at_state::Tier tiers[]={
        slotv2::at_state::Tier::Lower,
        slotv2::at_state::Tier::Middle,
        slotv2::at_state::Tier::Upper
    };
    for(const auto tier:tiers) {
        double average=0;
        for(int j=0;j<4;++j) {
            slotv2::at_state::State s{};
            s.tier=tier;
            s.table=tables[j];
            s.cold=false;
            const auto warm=slotv2::at_lottery::regionsFor(s);
            s.cold=true;
            const auto cold=slotv2::at_lottery::regionsFor(s);
            ok=ok && warm.fall==slotv2::kRngSpace/400u;
            ok=ok && cold.fall==warm.fall;
            const uint32_t expected_chain=slotv2::kRngSpace/
                (tier==slotv2::at_state::Tier::Lower ? 500u :
                (tier==slotv2::at_state::Tier::Middle ? 450u : 400u));
            ok=ok && warm.chain_zone==expected_chain;
            ok=ok && cold.chain_zone==expected_chain;
            average+=weights[j]*(0.4*warm.eligibleTotal()
                                  +0.6*cold.eligibleTotal())/n;
        }
        const double target=tier==slotv2::at_state::Tier::Lower
            ? 1.0/80.0 : 1.0/50.0;
        ok=ok && std::abs(average-target)<0.000005;
    }
    if (!ok) {
        std::cerr << "slot_v2_setting6_rates_test: FAILED\n";
        return 1;
    }
    std::cout << "slot_v2_setting6_rates_test: OK\n";
    return 0;
}
