#pragma once
#include <stdint.h>
#include "../shared/types.hpp"

namespace slotv2::reel_validator {

enum ResultBits : uint32_t {
    LeftCherryHidePossible = 1u << 0,
    LeftBellGuaranteed = 1u << 1,
    LeftReplayGuaranteed = 1u << 2
};

// 現在登録されている左21コマが停止制御条件を満たすか全21押下位置で検査する。
// 3bitすべて立てば、現時点の左リール基本条件を通過。
uint32_t validateLeft();

} // namespace slotv2::reel_validator
