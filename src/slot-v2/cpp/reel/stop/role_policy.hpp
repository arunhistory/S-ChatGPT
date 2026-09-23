#pragma once
#include <stdint.h>
#include "../shared/types.hpp"

namespace slotv2::role_policy {

enum class StopPolicy : uint8_t {
    Plain = 0,
    Assist = 1,
    EyeStop = 2,
    SubstituteCapable = 3
};

StopPolicy stopPolicy(RoleFlag role);

} // namespace slotv2::role_policy
