#include "index.hpp"

namespace slotv2::ceiling_catalog {

bool contains(uint16_t games) {
    return indexOf(games) >= 0;
}

int indexOf(uint16_t games) {
    for (uint8_t i = 0; i < kCount; ++i) {
        if (kValues[i] == games) return static_cast<int>(i);
    }
    return -1;
}

} // namespace slotv2::ceiling_catalog
