#pragma once
#include "../machine-state/index.hpp"
#include "../pending-event/index.hpp"
#include "../normal-mode/index.hpp"
#include "../normal-flow/index.hpp"
#include "../normal-at-trigger/index.hpp"
#include "../normal-hit-entry/index.hpp"

namespace slotv2::normal_flow_transition {

enum class Outcome : uint8_t {
    None = 0,
    CZStarted = 1,
    BonusQueued = 2,
    ATQueued = 3
};

struct Result {
    Outcome outcome{Outcome::None};
    normal_at_trigger::Result guarantee{};
    normal_hit_entry::Result entry{};
};

Result apply(
    machine_state::State& machine,
    pending_event::State& pending,
    normal_mode::Mode mode,
    normal_flow::Reward reward
);

} // namespace slotv2::normal_flow_transition
