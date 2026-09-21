#pragma once
#include "../reel-strip/index.hpp"
#include "../stop-shared/index.hpp"

namespace slotv2::stop_rules {

bool visibleHas(const reel_strip::StripView& strip, uint8_t center, Symbol symbol);
bool centerIs(const reel_strip::StripView& strip, uint8_t center, Symbol symbol);
bool leftCherrySafe(const stop_shared::Context& ctx, const reel_strip::StripView& strip, uint8_t candidate);

} // namespace slotv2::stop_rules
