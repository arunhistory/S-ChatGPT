#pragma once
#include <stdint.h>
#include "../shared/types.hpp"

namespace slotv2::reel_validator {

enum ResultBits : uint32_t {
    LeftCherryHidePossible = 1u << 0,
    LeftBellGuaranteed = 1u << 1,
    LeftReplayGuaranteed = 1u << 2,
    LeftBarLandmarkPair = 1u << 3
};

enum AssistResultBits : uint32_t {
    AssistStripDefined = 1u << 0,
    AssistBellGuaranteed = 1u << 1,
    AssistReplayGuaranteed = 1u << 2
};

// 現在登録されている左21コマが停止制御条件を満たすか全21押下位置で検査する。
// 4bitすべて立てば、現時点の左リール基本条件を通過。
uint32_t validateLeft();
uint32_t validateAssist(ReelId reel);

// 21bit: bitN=押下位置Nで0〜4コマ内に合法取得候補が無い。
// 左リールは非チェリー成立時の可視🍒禁止も同時に判定する。
uint32_t assistFailureMask(ReelId reel, RoleFlag role);

uint32_t readyMask();

} // namespace slotv2::reel_validator
