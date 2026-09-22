#include <iostream>
#include "special-zone-pending/index.hpp"

int main() {
    bool ok = true;
    slotv2::pending_event::State pending{};

    slotv2::special_zone_pending::publish(
        slotv2::special_zone::HitResult::AddGames,
        pending
    );

    ok = ok && slotv2::pending_event::has(
        pending,
        slotv2::pending_event::SpecialZoneAddGames
    );

    slotv2::special_zone_pending::publish(
        slotv2::special_zone::HitResult::Bonus,
        pending
    );

    ok = ok && slotv2::pending_event::has(
        pending,
        slotv2::pending_event::SpecialZoneBonus
    );

    const uint32_t before = pending.bits;
    slotv2::special_zone_pending::publish(
        slotv2::special_zone::HitResult::None,
        pending
    );
    ok = ok && pending.bits == before;

    if (!ok) {
        std::cerr << "slot_v2_special_zone_pending_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_special_zone_pending_test: OK\n";
    return 0;
}
