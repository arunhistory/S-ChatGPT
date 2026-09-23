#pragma once
#include <stdint.h>
#include "../shared/rng.hpp"

namespace slotv2::normal_shortening {

uint16_t weak(Rng& rng);
uint16_t strong(Rng& rng);
uint16_t continuous(Rng& rng);

} // namespace slotv2::normal_shortening
