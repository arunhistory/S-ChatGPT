#include <iostream>
#include "core/preflight.hpp"

int main() {
    const uint32_t bits = slotv2::preflight::check();

    bool ok = true;

    ok = ok && (bits & slotv2::preflight::LeftDefined);
    ok = ok && (bits & slotv2::preflight::MiddleDefined);
    ok = ok && (bits & slotv2::preflight::RightDefined);
    ok = ok && (bits & slotv2::preflight::LeftCherryHide);
    ok = ok && (bits & slotv2::preflight::LeftBellAssist);
    ok = ok && (bits & slotv2::preflight::LeftReplayAssist);
    ok = ok && (bits & slotv2::preflight::MiddleBellAssist);
    ok = ok && (bits & slotv2::preflight::MiddleReplayAssist);
    ok = ok && (bits & slotv2::preflight::RightBellAssist);
    ok = ok && (bits & slotv2::preflight::RightReplayAssist);
    ok = ok && (bits & slotv2::preflight::Playable);

    if (!ok) {
        std::cerr << "slot_v2_preflight_test: FAILED bits=" << bits << "\n";
        return 1;
    }

    std::cout << "slot_v2_preflight_test: OK bits=" << bits << "\n";
    return 0;
}
