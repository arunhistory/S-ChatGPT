#pragma once
#include "reel/stop/stop_types.hpp"

namespace slotv2::assist_integrity {

// 第2/第3停止時に、すでに止まったリールが内部アシスト役の
// 中段成立条件を維持しているか確認する。
bool stoppedCompatible(const stop_shared::Context& ctx);

} // namespace slotv2::assist_integrity
