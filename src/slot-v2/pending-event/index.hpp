#pragma once
#include <stdint.h>

namespace slotv2::pending_event {

enum Bits : uint32_t {
    None             = 0u,
    BellFiveAT       = 1u << 0,
    CZThreeMissHit   = 1u << 1,
    NextHitAT        = 1u << 2,

    ATHit            = 1u << 3,
    ATFall           = 1u << 4,
    ATAddGames       = 1u << 5,
    ATSpecial        = 1u << 6,
    ATEpisode        = 1u << 7,
    ATUpperSpecial   = 1u << 8,
    ATMultiple       = 1u << 9,

    SectionTierUp    = 1u << 10,
    SectionSpecial   = 1u << 11,
    SectionUpperSpec = 1u << 12,

    CZHit            = 1u << 13,
    ATWindowEmpty    = 1u << 14,
    ATStockAvailable = 1u << 15,

    BonusComplete       = 1u << 16,
    BonusEpisodeUpgrade= 1u << 17,

    UpperComebackHit    = 1u << 18
};

struct State {
    uint32_t bits{0};
};

void add(State& state, Bits event);
bool has(const State& state, Bits event);
bool consume(State& state, Bits event);
void clear(State& state);

} // namespace slotv2::pending_event
