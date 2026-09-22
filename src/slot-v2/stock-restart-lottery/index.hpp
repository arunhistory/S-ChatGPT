#pragma once
#include <stdint.h>
#include "../shared/rng.hpp"

namespace slotv2::stock_restart_lottery {

// Restart-set distribution accepted for the Japan-target build.
// 20G 20%, 30G 25%, 40G 20%, 50G 15%, 75G 8%,
// 100G 6%, 150G 3.5%, 200G 2.5%. Mean 49.25G.
// Roll uses 0..1999 to represent 0.05% units exactly.
uint16_t fromRoll(uint16_t roll_0_to_1999);
uint16_t draw(Rng& rng);

} // namespace slotv2::stock_restart_lottery
