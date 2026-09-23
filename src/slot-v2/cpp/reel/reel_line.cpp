#include "reel/reel_line.hpp"
#include "reel/reel_strip.hpp"

namespace slotv2::line {

MiddleLine read(uint8_t left_pos, uint8_t middle_pos, uint8_t right_pos) {
    const auto l = reel_strip::get(ReelId::Left);
    const auto m = reel_strip::get(ReelId::Middle);
    const auto r = reel_strip::get(ReelId::Right);

    if (!l.data || !m.data || !r.data) return {};

    MiddleLine out{};
    out.left = l.data[left_pos % l.size];
    out.middle = m.data[middle_pos % m.size];
    out.right = r.data[right_pos % r.size];
    out.ready = true;
    return out;
}

} // namespace slotv2::line
