#include "index.hpp"

namespace slotv2::accounting {

Result debit(State& accounting, section::State& section_state, int medals) {
    if (medals < 0) medals = 0;

    accounting.total_bet += medals;
    accounting.total_diff -= medals;
    const auto sr = section::apply(section_state, -static_cast<int64_t>(medals));

    return {
        sr,
        accounting.total_bet,
        accounting.total_payout,
        accounting.total_diff
    };
}

Result credit(State& accounting, section::State& section_state, int medals) {
    if (medals < 0) medals = 0;

    accounting.total_payout += medals;
    accounting.total_diff += medals;
    const auto sr = section::apply(section_state, static_cast<int64_t>(medals));

    return {
        sr,
        accounting.total_bet,
        accounting.total_payout,
        accounting.total_diff
    };
}

} // namespace slotv2::accounting
