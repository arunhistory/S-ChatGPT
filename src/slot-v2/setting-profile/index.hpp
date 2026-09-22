#pragma once
#include <stdint.h>

namespace slotv2::setting_profile {

struct NormalRaw {
    uint32_t at_threshold_27{0};
    uint32_t bonus_threshold_27{0};
    uint32_t cz_threshold_27{0};
};

uint8_t clampSetting(uint8_t setting);
NormalRaw normalRaw(uint8_t setting);

} // namespace slotv2::setting_profile
