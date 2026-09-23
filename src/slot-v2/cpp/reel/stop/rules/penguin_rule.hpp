#pragma once
#include "reel/stop/rules/common_rule.hpp"

namespace slotv2::stop_rules::penguin {
bool accepts(const stop_shared::Context& ctx, const reel_strip::StripView& strip, uint8_t candidate);
}
