#include <iostream>
#include "preflight/index.hpp"

int main() {
    const uint32_t bits = slotv2::preflight::check();

    bool ok = true;

    // 現時点の正式リールは左のみ。中・右候補を勝手にruntimeへ昇格させない。
    ok = ok && (bits & slotv2::preflight::LeftDefined);
    ok = ok && !(bits & slotv2::preflight::MiddleDefined);
    ok = ok && !(bits & slotv2::preflight::RightDefined);

    // 未確定リールがあるため、正式なPlayableにはしてはいけない。
    ok = ok && !(bits & slotv2::preflight::Playable);

    if (!ok) {
        std::cerr << "slot_v2_preflight_test: FAILED bits=" << bits << "\n";
        return 1;
    }

    std::cout << "slot_v2_preflight_test: OK bits=" << bits << "\n";
    return 0;
}
