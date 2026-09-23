#include "reel/reel_read.hpp"
#include "reel/reel_strip.hpp"

namespace slotv2::reel_read {

Symbol at(ReelId reel, uint8_t position) {
    const auto strip = reel_strip::get(reel);
    if (!strip.data || strip.size == 0) return Symbol::Unknown;
    return strip.data[position % strip.size];
}

Symbol visible(ReelId reel, uint8_t center_position, int8_t row_offset) {
    const auto strip = reel_strip::get(reel);
    if (!strip.data || strip.size == 0) return Symbol::Unknown;

    int p = static_cast<int>(center_position) + static_cast<int>(row_offset);
    while (p < 0) p += strip.size;
    p %= strip.size;
    return strip.data[p];
}

} // namespace slotv2::reel_read
