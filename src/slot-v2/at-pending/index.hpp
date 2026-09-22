#pragma once
#include "../at-resolution/index.hpp"
#include "../pending-event/index.hpp"

namespace slotv2::at_pending {

// AT内部抽選の確定分類をpending eventへ反映。
// Singleは対応イベントを1つだけ、Multipleは複数成立保留を立てる。
// 優先順位や実処理はここでは決めない。
void publish(
    const at_resolution::Result& resolution,
    pending_event::State& pending
);

} // namespace slotv2::at_pending
