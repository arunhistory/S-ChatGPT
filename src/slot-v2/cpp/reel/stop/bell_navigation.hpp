#pragma once
#include <stdint.h>
#include "shared/rng.hpp"
#include "shared/types.hpp"
#include "reel/stop/navigation.hpp"

namespace slotv2::bell_navigation {

struct Plan {
    bool active{false};
    uint8_t order_index{0};
    navigation::Order order{};
};

// AT/ボーナス等の上位状態が navigation_enabled=true を渡した時だけ生成。
// 通常時からTSが勝手に有効化する入口は持たせない。
Plan make(Rng& rng, RoleFlag role, bool navigation_enabled);

} // namespace slotv2::bell_navigation
