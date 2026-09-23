#pragma once
#include <stdint.h>
#include "../shared/types.hpp"

namespace slotv2::stop_shared {

enum class ResolveStatus : uint8_t {
    Ok = 0,
    StripUndefined = 1,
    NoLegalCandidate = 2,
    InvalidReel = 3,
    SpecialControlPending = 4,
    RoleMissed = 5,
    SubstituteStop = 6,
    AssistGap = 7
};

struct Context {
    RoleFlag role{RoleFlag::None};
    ReelId reel{ReelId::Left};
    uint8_t pressed_position{0};
    uint8_t stop_order{0}; // 0=first, 1=second, 2=third
    bool stopped[3]{false,false,false};
    uint8_t stopped_position[3]{0,0,0};
};

struct Result {
    ResolveStatus status{ResolveStatus::NoLegalCandidate};
    uint8_t final_position{0};
    uint8_t slip{0};
};

} // namespace slotv2::stop_shared
