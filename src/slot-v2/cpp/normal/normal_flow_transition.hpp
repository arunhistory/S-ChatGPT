#pragma once
#include "core/machine_state.hpp"
#include "core/pending_event.hpp"
#include "normal/normal_mode.hpp"
#include "normal/normal_flow.hpp"
#include "normal/normal_at_trigger.hpp"
#include "normal/normal_hit_entry.hpp"

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
