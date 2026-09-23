#pragma once
#include "core/machine_state.hpp"
#include "core/pending_event.hpp"
#include "at/at_resolution.hpp"

namespace slotv2::at_single_transition {

struct Result {
    bool applied{false};
    bool regular_bonus_started{false};
    bool episode_bonus_started{false};
    bool special_started{false};
};

// 優先順位不要な「単独成立」だけを処理する。
// Hit/Episode -> 1G AT omen first. Special -> regular special zone 5G.
// Fall/AddGames are handled by at-internal-transition.
// UpperSpecial is handled by upper-special-transition.
// This module owns only Hit/Episode/Special single-event entry routing.
Result apply(
    machine_state::State& machine,
    pending_event::State& pending,
    const at_resolution::Result& resolution
);

} // namespace slotv2::at_single_transition
