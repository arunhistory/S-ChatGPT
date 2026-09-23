#pragma once
#include <stdint.h>

namespace slotv2 {

class Rng {
public:
    explicit Rng(uint64_t seed = 0x5343484154475054ULL) { reset(seed); }
    void reset(uint64_t seed);
    uint64_t next64();
    uint32_t next27();
    uint32_t uniformBelow(uint32_t bound);
    bool oneIn(uint32_t denominator);

private:
    uint64_t state_{0};
};

} // namespace slotv2
