#include "strong-chance.hpp"

namespace slotv2::stop_rules::strong_chance {

bool accepts(const stop_shared::Context&, const reel_strip::StripView&, uint8_t) {
    // 代用停止対応役。正式停止形が決まるまでは「直取得」と判定しない。
    return false;
}

} // namespace slotv2::stop_rules::strong_chance
