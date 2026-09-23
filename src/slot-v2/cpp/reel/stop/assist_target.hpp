#pragma once
#include "shared/types.hpp"

namespace slotv2::assist_target {

// 中段1ラインでアシスト役が許容する図柄。
// 右🟥7はベル成立時だけ true になる。
bool accepts(RoleFlag role, ReelId reel, Symbol symbol);

} // namespace slotv2::assist_target
