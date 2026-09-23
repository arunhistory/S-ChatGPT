#pragma once
#include <stdint.h>
#include "../shared/rng.hpp"
#include "../upper-comeback/index.hpp"

namespace slotv2::upper_comeback_cycle {

struct Result {
    bool active_before{false};
    uint8_t games_before{0};
    uint8_t games_after{0};
    bool ended{false};
    bool hit{false};
};

// 上位AT終了後64G窓の1G進行。
// 最終G消化時だけ20%引戻し判定を1回行う。
Result playOne(
    Rng& rng,
    upper_comeback::State& state
);

} // namespace slotv2::upper_comeback_cycle
