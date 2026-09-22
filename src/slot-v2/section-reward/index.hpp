#pragma once
#include <stdint.h>
#include "../shared/rng.hpp"
#include "../at-state/index.hpp"

namespace slotv2::section_reward {

enum class Kind : uint8_t {
    None = 0,
    TierUp = 1,
    Special = 2,
    UpperSpecial = 3
};

struct Result {
    Kind kind{Kind::None};
    uint8_t preference_level{0};
};

// preference_level:
// 0 = stock 0
// 1 = stock 1-2
// 2 = stock 3-4
// 3 = stock 5+
//
// Lower/Middle:
// 0:0.5% / 1:10% / 2:25% / 3:50% で昇格
//
// Upper:
// 0:0% / 1:10% / 2:50% / 3:75% で特化
// 特化当選の1/3で上位特化
Result draw(Rng& rng, at_state::Tier tier, uint8_t preference_level);

} // namespace slotv2::section_reward
