#pragma once
#include <stdint.h>

namespace slotv2 {

class Rng {
public:
    // Constant initialization prevents wasm32 --no-entry from rerunning a
    // dynamic global constructor on exported calls. Same seed mapping as reset().
    explicit constexpr Rng(uint64_t seed = 0x5343484154475054ULL)
        : state_(seed ? seed : 0x9E3779B97F4A7C15ULL) {}
    void reset(uint64_t seed);
    uint64_t next64();
    uint32_t next27();
    uint32_t uniformBelow(uint32_t bound);
    bool oneIn(uint32_t denominator);

private:
    uint64_t state_{0};
};

} // namespace slotv2
