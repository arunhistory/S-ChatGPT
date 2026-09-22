#pragma once
#include <stdint.h>

namespace slotv2::preflight {

enum Bits : uint32_t {
    LeftDefined        = 1u << 0,
    MiddleDefined      = 1u << 1,
    RightDefined       = 1u << 2,

    LeftCherryHide     = 1u << 3,
    LeftBellAssist     = 1u << 4,
    LeftReplayAssist   = 1u << 5,
    LeftBarLandmarks   = 1u << 6,

    MiddleBellAssist   = 1u << 7,
    MiddleReplayAssist = 1u << 8,

    RightBellAssist    = 1u << 9,
    RightReplayAssist  = 1u << 10,

    Playable           = 1u << 31
};

uint32_t check();

} // namespace slotv2::preflight
