#pragma once
#include <stdint.h>
#include "../shared/types.hpp"
#include "../stock-count-lottery/index.hpp"

namespace slotv2::special_result {

enum class EntryTarget : uint8_t {
    None = 0,
    MiddleAT = 1,
    UpperAT = 2
};

struct Result {
    SpecialHit hit{SpecialHit::None};
    EntryTarget target{EntryTarget::None};
    stock_count_lottery::Profile stock_profile{stock_count_lottery::Profile::None};
    bool freeze{false};
};

// 特殊直撃抽選の結果を、ゲーム内部の確定権利へ変換する。
// TSはこの結果を変更しない。
Result resolve(SpecialHit hit);

} // namespace slotv2::special_result
