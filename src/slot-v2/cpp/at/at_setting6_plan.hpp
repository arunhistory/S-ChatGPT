#pragma once
#include <stdint.h>
#include "shared/types.hpp"
#include "at/at_state.hpp"

// Frozen setting-6 AT event allocation plan. This header does not alter the
// live lottery until the separately approved 5G chain-zone runtime is wired.
// All tier/table distributions preserve the ratios of EXISTING positive events.
// Fall remains a separate 1/400 draw region, not part of the 1/80 or 1/50
// positive-event aggregate. Chain zone IS included in that aggregate.
// These nominal rates apply to an uncooled AT game; cold-state scaling and
// time spent in special subgames change the realized long-run frequency.
namespace slotv2::at_setting6_plan {

struct Counts {
    uint32_t hit{};
    uint32_t fall{};
    uint32_t add_games{};
    uint32_t special{};
    uint32_t episode{};
    uint32_t upper_special{};
    uint32_t chain_zone{};

    constexpr uint32_t positive() const {
        return hit + add_games + special + episode + upper_special + chain_zone;
    }
    constexpr uint32_t all() const { return positive() + fall; }
};

constexpr uint32_t nearestCount(uint32_t denominator) {
    return (kRngSpace + denominator / 2u) / denominator;
}

constexpr Counts forTierTable(at_state::Tier tier, at_state::Table table) {
    const bool higher = tier != at_state::Tier::Lower;
    // Original v2 positive-event denominator table. Do not fold in Fall.
    uint32_t denominators[5]{};
    switch (table) {
        case at_state::Table::Heaven:
            if (higher) {
                denominators[0]=48; denominators[1]=145;
                denominators[2]=340; denominators[3]=600;
            } else {
                denominators[0]=100; denominators[1]=300;
                denominators[2]=700; denominators[3]=800;
            }
            break;
        case at_state::Table::SuperHeaven:
            if (higher) {
                denominators[0]=95; denominators[1]=60;
                denominators[2]=340; denominators[3]=500;
            } else {
                denominators[0]=200; denominators[1]=125;
                denominators[2]=700; denominators[3]=700;
            }
            break;
        case at_state::Table::Specialized:
            if (higher) {
                denominators[0]=170; denominators[1]=260;
                denominators[2]=120; denominators[4]=950;
            } else {
                denominators[0]=350; denominators[1]=550;
                denominators[2]=250; denominators[4]=2000;
            }
            break;
        case at_state::Table::Normal:
        default:
            if (higher) {
                denominators[0]=95; denominators[1]=145;
                denominators[2]=340;
            } else {
                denominators[0]=200; denominators[1]=300;
                denominators[2]=700;
            }
            break;
    }

    const uint32_t aggregateDenominator = higher ? 50u : 80u;
    const uint32_t chainDenominator =
        tier == at_state::Tier::Lower ? 500u
        : (tier == at_state::Tier::Middle ? 450u : 400u);
    Counts out{};
    out.fall = nearestCount(400u);
    out.chain_zone = nearestCount(chainDenominator);
    const uint32_t oldCounts[5] = {
        denominators[0] ? kRngSpace/denominators[0] : 0u,
        denominators[1] ? kRngSpace/denominators[1] : 0u,
        denominators[2] ? kRngSpace/denominators[2] : 0u,
        denominators[3] ? kRngSpace/denominators[3] : 0u,
        denominators[4] ? kRngSpace/denominators[4] : 0u
    };
    uint64_t weightSum = 0u;
    int lastNonZero = -1;
    for (int i=0; i<5; ++i) {
        weightSum += oldCounts[i];
        if (oldCounts[i]) lastNonZero=i;
    }

    const uint32_t otherTarget =
        nearestCount(aggregateDenominator) - out.chain_zone;
    uint32_t scaled[5]{};
    uint32_t remaining = otherTarget;
    for (int i=0; i<5; ++i) {
        if (!oldCounts[i]) continue;
        const uint32_t n = i==lastNonZero
            ? remaining
            : static_cast<uint32_t>(
                (static_cast<uint64_t>(otherTarget)*oldCounts[i])/weightSum
            );
        scaled[i]=n;
        remaining -= n;
    }
    out.hit=scaled[0];
    out.add_games=scaled[1];
    out.special=scaled[2];
    out.episode=scaled[3];
    out.upper_special=scaled[4];
    return out;
}

static_assert(forTierTable(at_state::Tier::Lower, at_state::Table::Normal)
              .positive() == nearestCount(80u));
static_assert(forTierTable(at_state::Tier::Middle, at_state::Table::Normal)
              .positive() == nearestCount(50u));
static_assert(forTierTable(at_state::Tier::Upper, at_state::Table::Normal)
              .positive() == nearestCount(50u));

} // namespace slotv2::at_setting6_plan
