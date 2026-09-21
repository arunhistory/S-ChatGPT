#pragma once
#include <stdint.h>
#include "../machine-state/index.hpp"
#include "../special-result/index.hpp"

namespace slotv2::special_apply {

struct Result {
    bool applied{false};
    bool at_started{false};
    at_state::Tier tier{at_state::Tier::Lower};
    uint32_t stock_added{0};
    bool freeze{false};
};

// 特殊直撃の確定権利を機械状態へ反映する。
// 演出完了後に一度だけ呼び、TS側ではAT/ストックを変更しない。
Result apply(machine_state::State& machine, const special_result::Result& special);

} // namespace slotv2::special_apply
