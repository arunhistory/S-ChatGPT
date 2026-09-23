#include <iostream>
#include "normal/ceiling_catalog.hpp"
#include "normal/normal_progress_view.hpp"

int main() {
    bool ok = true;

    ok = ok && slotv2::ceiling_catalog::kCount == 22u;
    for (uint8_t i = 0; i < slotv2::ceiling_catalog::kCount; ++i) {
        ok = ok && slotv2::ceiling_catalog::contains(
            slotv2::ceiling_catalog::kValues[i]
        );
    }

    ok = ok && !slotv2::ceiling_catalog::contains(0);
    ok = ok && !slotv2::ceiling_catalog::contains(999);

    slotv2::normal_progress::State p{};
    p.cz_misses = 2;
    p.normal_hits_without_at = 4;
    p.bell9_streak = 3;
    p.next_hit_at_guaranteed = true;

    const uint32_t packed = slotv2::normal_progress_view::pack(p);
    ok = ok && (packed & 0xffu) == 2u;
    ok = ok && ((packed >> 8) & 0xffu) == 4u;
    ok = ok && ((packed >> 16) & 0xffu) == 3u;
    ok = ok && ((packed >> 24) & 1u) == 1u;

    if (!ok) {
        std::cerr << "slot_v2_catalog_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_catalog_test: OK\n";
    return 0;
}
