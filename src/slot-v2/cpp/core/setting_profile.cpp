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

NormalRaw normalRaw(uint8_t setting) {
    switch (clampSetting(setting)) {
        case 1: return {11623u, 424392u, 191740u};
        case 2: return {78694u, 165226u, 280222u};
        case 3: return {86265u, 177925u, 284764u};
        case 4: return {93381u, 190517u, 290987u};
        case 5: return {100772u, 204521u, 299714u};
        // Setting 6 only: approved *observable* raw rewards per eligible normal draw.
        // Raw draw priority is AT -> Bonus -> CZ (three independent RNG draws),
        // so lower-priority thresholds compensate for the probability that
        // an earlier reward has already won this game.
        // 1/2650 lower AT, 1/1700 bonus, 1/1150 CZ, excluding ceilings etc.
        case 6: return {50648u, 78981u, 116824u};
        case 7:
        default:
            return {132579u, 257659u, 324197u};
    }
}

} // namespace slotv2::setting_profile
