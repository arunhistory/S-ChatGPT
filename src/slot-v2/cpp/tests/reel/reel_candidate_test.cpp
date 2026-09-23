#include <iostream>
#include "reel/stop/reel_candidate.hpp"
#include "reel/stop/assist_target.hpp"

namespace {

bool guaranteed(
    slotv2::reel_strip::StripView strip,
    slotv2::ReelId reel,
    slotv2::RoleFlag role
) {
    for (int pressed = 0; pressed < strip.size; ++pressed) {
        bool found = false;
        for (int slip = 0; slip <= slotv2::kMaxSlip; ++slip) {
            int p = pressed - slip;
            while (p < 0) p += strip.size;
            p %= strip.size;

            if (slotv2::assist_target::accepts(role, reel, strip.data[p])) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }
    return true;
}

}

int main() {
    bool ok = true;

    const auto m = slotv2::reel_candidate::middle();
    const auto r = slotv2::reel_candidate::right();

    ok = ok && m.size == 21 && r.size == 21;
    ok = ok && guaranteed(m, slotv2::ReelId::Middle, slotv2::RoleFlag::Bell9);
    ok = ok && guaranteed(m, slotv2::ReelId::Middle, slotv2::RoleFlag::Replay);
    ok = ok && guaranteed(r, slotv2::ReelId::Right, slotv2::RoleFlag::Bell9);
    ok = ok && guaranteed(r, slotv2::ReelId::Right, slotv2::RoleFlag::Replay);

    // 右11→12→13は 🟦7 → 🟥7 → BAR を維持。
    ok = ok
        && r.data[10] == slotv2::Symbol::Blue7
        && r.data[11] == slotv2::Symbol::Red7
        && r.data[12] == slotv2::Symbol::Bar;

    if (!ok) {
        std::cerr << "slot_v2_reel_candidate_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_reel_candidate_test: OK\n";
    return 0;
}
