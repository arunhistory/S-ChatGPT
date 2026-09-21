#pragma once
#include "../shared/types.hpp"

namespace slotv2::reel_strip {

struct StripView {
    const Symbol* data;
    uint8_t size;
};

// 現時点で確定済みなのは左リールのみ。
// 中・右は配置確定前なので、勝手な仮配列を新制御へ持ち込まない。
StripView get(ReelId reel);

} // namespace slotv2::reel_strip
