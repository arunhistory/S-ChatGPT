#pragma once
#include "shared/types.hpp"
#include "reel/reel_acquisition.hpp"
#include "normal/normal_progress.hpp"
#include "core/pending_event.hpp"

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
