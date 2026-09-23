#include "core/setting_profile.hpp"

namespace slotv2::setting_profile {

uint8_t clampSetting(uint8_t setting) {
    if (setting < 1u) return 1u;
    if (setting > 7u) return 7u;
    return setting;
}

NormalRaw normalRaw(uint8_t setting) {
    switch (clampSetting(setting)) {
        case 1: return {11623u, 424392u, 191740u};
        case 2: return {78694u, 165226u, 280222u};
        case 3: return {86265u, 177925u, 284764u};
        case 4: return {93381u, 190517u, 290987u};
        case 5: return {100772u, 204521u, 299714u};
        case 6: return {108269u, 216538u, 305040u};
        case 7:
        default:
            return {132579u, 257659u, 324197u};
    }
}

} // namespace slotv2::setting_profile
