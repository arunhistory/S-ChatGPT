#pragma once
#include "../shared/rng.hpp"

namespace slotv2::stock_lottery {

// AT内部の当たり成立時に10%でストック獲得。
bool onHit(Rng& rng);

} // namespace slotv2::stock_lottery
