#pragma once
#include "../machine-state/index.hpp"
#include "../pending-event/index.hpp"

namespace slotv2::normal_at_trigger {

struct Result {
    bool started{false};
    bool from_bell_five{false};
    bool from_next_hit_guarantee{false};
};

// 通常時9枚ベル5連のAT当選を確定反映。
// 指定がないため通常AT=下位100Gとして開始する。
Result applyBellFive(
    machine_state::State& machine,
    pending_event::State& pending
);

// 「通常当たり5連続後の次回AT確定」を、次の通常当たりへ反映。
// base_was_at=trueなら既にAT当選済みなので保証は消費しない。
Result applyNextHitGuarantee(
    machine_state::State& machine,
    pending_event::State& pending,
    bool base_was_at
);

} // namespace slotv2::normal_at_trigger
