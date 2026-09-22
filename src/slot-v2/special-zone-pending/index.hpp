#pragma once
#include "../special-zone/index.hpp"
#include "../pending-event/index.hpp"

namespace slotv2::special_zone_pending {

void publish(
    special_zone::HitResult result,
    pending_event::State& pending
);

} // namespace slotv2::special_zone_pending
