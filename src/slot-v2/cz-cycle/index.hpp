#pragma once
#include <stdint.h>
#include "../shared/rng.hpp"
#include "../cz-state/index.hpp"

namespace slotv2::cz_cycle {

struct Result {
    bool active{false};
    bool base_hit{false};
    uint8_t games_left{0};
    bool ended{false};
};

// CZ中の1ゲーム。
// 基本1/100のみ。小役補正・AT高確側は未確定なのでここでは加えない。
// 基本当選した時点でCZはそのゲームで終了する。
Result playOne(Rng& rng, cz_state::State& state);

} // namespace slotv2::cz_cycle
