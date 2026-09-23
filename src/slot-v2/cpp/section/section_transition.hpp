#pragma once
#include "core/machine_state.hpp"
#include "core/pending_event.hpp"
#include "section/section_flow.hpp"

namespace slotv2::section_transition {

struct Result {
    bool applied{false};
    bool tier_changed{false};
    at_state::Tier before{at_state::Tier::Lower};
    at_state::Tier after{at_state::Tier::Lower};
    bool special_started{false};
    bool upper_special_pending{false};
};

// 区間報酬を、仕様が確定している範囲だけ機械状態へ反映する。
// ・TierUp: Lower->Middle / Middle->Upper
// ・Special: 通常特化5Gを開始
// ・UpperSpecial: pendingを残し、upper-special-transitionが90%継続特化へ接続
Result apply(
    machine_state::State& machine,
    pending_event::State& pending,
    const section_flow::Result& flow
);

} // namespace slotv2::section_transition
