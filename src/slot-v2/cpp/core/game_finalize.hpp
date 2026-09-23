#pragma once
#include "../shared/types.hpp"
#include "../acquisition/index.hpp"
#include "../normal-progress/index.hpp"
#include "../pending-event/index.hpp"

namespace slotv2::game_finalize {

struct Result {
    bool bell9_acquired{false};
    bool bell5_at_triggered{false};
};

Result apply(
    RoleFlag internal_role,
    const acquisition::Result& acquisition,
    normal_progress::State& progress,
    pending_event::State& pending
);

} // namespace slotv2::game_finalize
