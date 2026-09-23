#pragma once
#include "reel/reel_strip.hpp"

namespace slotv2::reel_candidate {

// 開発用候補。runtimeの正式リールにはまだ接続しない。
// 中・右21コマを決める前に0〜4コマ保証を総当たり検証するためのデータ。
reel_strip::StripView middle();
reel_strip::StripView right();

} // namespace slotv2::reel_candidate
