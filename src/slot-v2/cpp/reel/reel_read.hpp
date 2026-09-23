#pragma once
#include "shared/types.hpp"

namespace slotv2::reel_read {

Symbol at(ReelId reel, uint8_t position);
Symbol visible(ReelId reel, uint8_t center_position, int8_t row_offset);

} // namespace slotv2::reel_read
