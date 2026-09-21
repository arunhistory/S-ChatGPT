#pragma once
#include <stdint.h>
#include "../shared/types.hpp"

namespace slotv2::special_result {

enum class EntryTarget : uint8_t {
    None = 0,
    MiddleAT = 1,
    UpperAT = 2
};

struct Result {
    SpecialHit hit{SpecialHit::None};
    EntryTarget target{EntryTarget::None};
    uint8_t stock{0};
    bool freeze{false};
};

// 特殊直撃抽選の結果を、ゲーム内部の確定権利へ変換する。
// TSはこの結果を変更しない。
Result resolve(SpecialHit hit);

} // namespace slotv2::special_result
