#include "index.hpp"

namespace slotv2::game_finalize {

Result apply(
    RoleFlag internal_role,
    const acquisition::Result& acquisition,
    normal_progress::State& progress,
    pending_event::State& pending
) {
    Result out{};

    out.bell9_acquired =
        internal_role == RoleFlag::Bell9
        && acquisition.status == acquisition::Status::Acquired
        && acquisition.medals == 9;

    if (!out.bell9_acquired) {
        normal_progress::onNonBell9(progress);
        return out;
    }

    if (normal_progress::onBell9(progress)) {
        pending_event::add(pending, pending_event::BellFiveAT);
        out.bell5_at_triggered = true;
    }

    return out;
}

} // namespace slotv2::game_finalize
