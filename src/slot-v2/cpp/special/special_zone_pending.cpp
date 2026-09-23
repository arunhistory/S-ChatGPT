#include "index.hpp"

namespace slotv2::special_zone_pending {

void publish(
    special_zone::HitResult result,
    pending_event::State& pending
) {
    switch (result) {
        case special_zone::HitResult::AddGames:
            pending_event::add(
                pending,
                pending_event::SpecialZoneAddGames
            );
            break;

        case special_zone::HitResult::Bonus:
            pending_event::add(
                pending,
                pending_event::SpecialZoneBonus
            );
            break;

        case special_zone::HitResult::None:
        default:
            break;
    }
}

} // namespace slotv2::special_zone_pending
