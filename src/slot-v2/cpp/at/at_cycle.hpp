#pragma once
#include <stdint.h>
#include "../shared/rng.hpp"
#include "../machine-state/index.hpp"
#include "../at-event/index.hpp"

namespace slotv2::at_cycle {

struct Result {
    bool active{false};
    at_event::Result raw{};
    int32_t games_left_before{0};
    int32_t games_left_after{0};
    bool window_empty_after_game{false};
};

// AT中の1ゲーム開始時に内部抽選を行い、そのゲーム分だけST残Gを1消費する。
// 同一Gで複数成立しうるため、優先順位・G加算量・転落処理は決めない。
// 0G到達時もAT自体はここでは終了させず、runtime側へ「窓が空になった」事実だけ返す。
Result beginGame(Rng& rng, machine_state::State& machine);

} // namespace slotv2::at_cycle
