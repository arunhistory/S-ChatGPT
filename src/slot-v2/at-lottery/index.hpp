#pragma once
#include <stdint.h>
#include "../shared/rng.hpp"

namespace slotv2::at_lottery {

struct Draw {
    bool hit{false};          // 1/200
    bool fall{false};         // 1/400
    bool add_games{false};    // 1/300
    bool special{false};      // 1/700
    bool episode{false};      // 1/1000
    bool upper_special{false};// 1/5000
};

// 各イベントは独立抽選。
// 同一Gで複数成立した場合の優先順位は、このモジュールでは決めない。
Draw drawBase(Rng& rng);

} // namespace slotv2::at_lottery
