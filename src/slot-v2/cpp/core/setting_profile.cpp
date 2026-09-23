#include "core/setting_profile.hpp"

namespace slotv2::setting_profile {

bool validSetting(uint8_t setting) {
    return setting >= kMinSetting && setting <= kMaxSetting;
}

uint8_t clampSetting(uint8_t setting) {
    if (setting < kMinSetting) return kMinSetting;
    if (setting > kMaxSetting) return kMaxSetting;
    return setting;
}

bool implementedSetting(uint8_t setting) {
    return setting == 6u;
}

NormalRaw normalRaw(uint8_t setting) {
    if (!implementedSetting(setting)) return {}; // 1-5 and 7: reserved, no speculative odds.

    // 2^27 draws per check, priority AT > BONUS > CZ.
    // Correct the later thresholds for mutually exclusive outcomes, so the
    // *observed raw results* (not just the underlying trials) approach:
    // AT=1/2650, BONUS=1/1700, CZ=1/1150, ceiling excluded.
    return {50648u, 78981u, 116824u};
}

} // namespace slotv2::setting_profile
