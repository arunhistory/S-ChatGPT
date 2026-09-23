#pragma once
#include "core/machine_state.hpp"
#include "core/pending_event.hpp"
#include "cz/cz_cycle.hpp"

namespace slotv2::cz_finalize {

enum class Outcome : uint8_t {
    None = 0,
    HitPending = 1,
    MissReturnNormal = 2,
    ThreeMissHitPending = 3
};

struct Result {
    Outcome outcome{Outcome::None};
    bool lever_blocked{false};
};

// CZ1Gの確定結果を機械状態へ反映する。
// ・CZ基本当選: CZHitをpending化し、次遷移が決まるまでCZ領域で停止。
// ・通常失敗: 10G終了後Normalへ戻す。
// ・3スルー: 当たり保証pendingを残し、次遷移が決まるまで停止。
// BONUS/ATのどちらへ入るかはここで勝手に決めない。
Result apply(
    machine_state::State& machine,
    pending_event::State& pending,
    const cz_cycle::Result& cycle
);

} // namespace slotv2::cz_finalize
