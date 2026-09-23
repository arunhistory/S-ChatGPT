#pragma once
#include "../normal-progress/index.hpp"
#include "core/pending_event.hpp"

namespace slotv2::progress_event {

// CZ結果そのものは別のC++抽選/状態制御が確定させる。
// ここは確定済みCZ結果を進行カウンタへ反映するだけ。
bool onCZResolved(
    normal_progress::State& progress,
    pending_event::State& pending,
    bool hit
);

// 通常当たりがATだったかどうかを、確定結果から進行カウンタへ反映。
void onNormalHitResolved(
    normal_progress::State& progress,
    pending_event::State& pending,
    bool was_at
);

// 次回AT保証を実際の通常当たりへ適用する時だけ消費する。
bool consumeNextHitAT(
    normal_progress::State& progress,
    pending_event::State& pending
);

} // namespace slotv2::progress_event
