#pragma once
#include "shared/types.hpp"

namespace slotv2::stop_candidate {

// STOPを押した瞬間の中段位置から、実機式の0〜4コマ候補だけを生成する。
// 役別の引き込み・蹴り判断は別プログラムが担当する。
StopCandidates build(uint8_t pressed_position);

} // namespace slotv2::stop_candidate
