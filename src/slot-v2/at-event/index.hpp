#pragma once
#include <stdint.h>
#include "../at-lottery/index.hpp"

namespace slotv2::at_event {

enum Bits : uint32_t {
    None         = 0u,
    Hit          = 1u << 0,
    Fall         = 1u << 1,
    AddGames     = 1u << 2,
    Special      = 1u << 3,
    Episode      = 1u << 4,
    UpperSpecial = 1u << 5
};

struct Result {
    uint32_t bits{0};
};

Result fromDraw(const at_lottery::Draw& draw);
bool has(const Result& result, Bits bit);

} // namespace slotv2::at_event
