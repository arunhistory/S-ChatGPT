#pragma once
#include <stdint.h>
#include "section/section_state.hpp"

namespace slotv2::accounting {

struct State {
    int64_t total_bet{0};
    int64_t total_payout{0};
    int64_t total_diff{0};
};

struct Result {
    section::ApplyResult section{};
    int64_t total_bet{0};
    int64_t total_payout{0};
    int64_t total_diff{0};
};

// ベット・払出を別コマンドにして、REPLAY等の扱いをTS側の推測で決めない。
Result debit(State& accounting, section::State& section, int medals);
Result credit(State& accounting, section::State& section, int medals);

} // namespace slotv2::accounting
