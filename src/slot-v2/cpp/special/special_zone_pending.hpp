#pragma once
#include "special/special_zone.hpp"
#include "core/pending_event.hpp"

namespace slotv2::special_zone_pending {

void publish(
    special_zone::HitResult result,
    pending_event::State& pending
);

} // namespace slotv2::special_zone_pending
