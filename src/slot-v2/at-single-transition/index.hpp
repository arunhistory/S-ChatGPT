#pragma once
#include "../machine-state/index.hpp"
#include "../pending-event/index.hpp"
#include "../at-resolution/index.hpp"

namespace slotv2::at_single_transition {

struct Result {
    bool applied{false};
    bool special_started{false};
};

// 優先順位不要な「単独成立」だけを処理する。
// 現時点で確定処理できるのは 1/700 特化 → 通常特化5G開始。
// Hit/Fall/AddGames/Episode/UpperSpecial は後続仕様が必要なのでpendingのまま。
Result apply(
    machine_state::State& machine,
    pending_event::State& pending,
    const at_resolution::Result& resolution
);

} // namespace slotv2::at_single_transition
