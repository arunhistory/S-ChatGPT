#pragma once
#include "../shared/types.hpp"

namespace slotv2::line {

struct MiddleLine {
    Symbol left{Symbol::Unknown};
    Symbol middle{Symbol::Unknown};
    Symbol right{Symbol::Unknown};
    bool ready{false};
};

MiddleLine read(uint8_t left_pos, uint8_t middle_pos, uint8_t right_pos);

} // namespace slotv2::line
