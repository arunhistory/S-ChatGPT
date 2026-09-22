#pragma once
#include <stdint.h>

namespace slotv2::ceiling_catalog {

static constexpr uint16_t kValues[] = {
    50, 100, 200, 250, 300, 400, 500, 600, 700, 750, 777,
    800, 900, 1000, 1100, 1200, 1250, 1300, 1350, 1400, 1450, 1500
};

static constexpr uint8_t kCount =
    static_cast<uint8_t>(sizeof(kValues) / sizeof(kValues[0]));

bool contains(uint16_t games);
int indexOf(uint16_t games);

} // namespace slotv2::ceiling_catalog
