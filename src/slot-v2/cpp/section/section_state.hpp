#pragma once
#include <stdint.h>

namespace slotv2::section {

static constexpr int64_t kCutGain = 2400;

struct State {
    int64_t current_diff{0};
    int64_t minimum_diff{0};
    uint64_t section_count{0};
};

struct ApplyResult {
    bool cut{false};
    int64_t before_reset_diff{0};
    int64_t before_reset_minimum{0};
    int64_t gain_from_minimum{0};
    uint64_t section_count{0};
};

// 有利区間は「開始点から+2400」ではなく、区間内最低差枚から+2400で切る。
// 例: -1000 -> +1400 で最低点から+2400到達。
ApplyResult apply(State& state, int64_t medal_delta);

} // namespace slotv2::section
