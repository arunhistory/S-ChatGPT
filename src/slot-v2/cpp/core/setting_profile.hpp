#pragma once
#include <stdint.h>

namespace slotv2::setting_profile {

static constexpr uint8_t kMinSetting = 1u;
static constexpr uint8_t kMaxSetting = 7u;
static constexpr uint8_t kDefaultSetting = 6u;

struct NormalRaw {
    uint32_t at_threshold_27{0};
    uint32_t bonus_threshold_27{0};
    uint32_t cz_threshold_27{0};
};

// The seven IDs remain reserved; only 6 has an approved playable profile.
bool validSetting(uint8_t setting);
bool implementedSetting(uint8_t setting);
uint8_t clampSetting(uint8_t setting);
NormalRaw normalRaw(uint8_t setting);

} // namespace slotv2::setting_profile
