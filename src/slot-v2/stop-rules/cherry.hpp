#pragma once
#include "common.hpp"

namespace slotv2::stop_rules::cherry {
bool accepts(const stop_shared::Context& ctx, const reel_strip::StripView& strip, uint8_t candidate);
}
