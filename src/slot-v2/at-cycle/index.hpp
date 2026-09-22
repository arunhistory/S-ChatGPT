#pragma once
#include "../shared/rng.hpp"
#include "../machine-state/index.hpp"
#include "../at-event/index.hpp"

namespace slotv2::at_cycle {

struct Result {
    bool active{false};
    at_event::Result raw{};
};

// AT中の1ゲーム開始時に、AT内部の独立抽選だけを実行する。
// 同一Gで複数成立しうるため、ここでは優先順位・G加算量・転落処理を決めない。
Result beginGame(Rng& rng, const machine_state::State& machine);

} // namespace slotv2::at_cycle
