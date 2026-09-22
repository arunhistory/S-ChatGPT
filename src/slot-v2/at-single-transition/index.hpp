#pragma once
#include "../machine-state/index.hpp"
#include "../pending-event/index.hpp"
#include "../at-resolution/index.hpp"

namespace slotv2::at_single_transition {

struct Result {
    bool applied{false};
    bool regular_bonus_started{false};
    bool episode_bonus_started{false};
    bool special_started{false};
};

// 優先順位不要な「単独成立」だけを処理する。
// Hit -> 通常BONUS50枚、Episode -> Episode80枚、Special -> 通常特化5G。
// Fall/AddGames are handled by at-internal-transition.
// UpperSpecial is handled by upper-special-transition.
// This module owns only Hit/Episode/Special single-event entry routing.
Result apply(
    machine_state::State& machine,
    pending_event::State& pending,
    const at_resolution::Result& resolution
);

} // namespace slotv2::at_single_transition
