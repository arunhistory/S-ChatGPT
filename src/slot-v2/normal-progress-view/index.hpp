#pragma once
#include <stdint.h>
#include "../normal-progress/index.hpp"

namespace slotv2::normal_progress_view {

// 0..7 CZ miss count
// 8..15 normal hits without AT
// 16..23 9-bell streak
// bit24 next-hit AT guarantee
uint32_t pack(const normal_progress::State& state);

} // namespace slotv2::normal_progress_view
