#pragma once
#include <stdint.h>
#include "../shared/rng.hpp"
#include "../shared/types.hpp"
#include "../normal-high/index.hpp"
#include "../normal-raw-lottery/index.hpp"

namespace slotv2::normal_flow {

enum class Reward : uint8_t {
    None = 0,
    LowerAT = 1,
    Bonus = 2,
    CZ = 3
};

struct Result {
    normal_high::Result high{};
    Reward reward{Reward::None};
};

Result draw(
    Rng& rng,
    normal_high::State& high,
    RoleFlag role,
    uint32_t actual_games,
    uint8_t setting
);

} // namespace slotv2::normal_flow
