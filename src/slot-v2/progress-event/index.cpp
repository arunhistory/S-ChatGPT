#include "index.hpp"

namespace slotv2::progress_event {

bool onCZResolved(
    normal_progress::State& progress,
    pending_event::State& pending,
    bool hit
) {
    const bool ceiling = normal_progress::onCZResult(progress, hit);
    if (ceiling) {
        pending_event::add(pending, pending_event::CZThreeMissHit);
    }
    return ceiling;
}

void onNormalHitResolved(
    normal_progress::State& progress,
    pending_event::State& pending,
    bool was_at
) {
    normal_progress::onNormalHit(progress, was_at);

    if (progress.next_hit_at_guaranteed) {
        pending_event::add(pending, pending_event::NextHitAT);
    }
}

bool consumeNextHitAT(
    normal_progress::State& progress,
    pending_event::State& pending
) {
    if (!normal_progress::consumeNextATGuarantee(progress)) return false;

    (void)pending_event::consume(
        pending,
        pending_event::NextHitAT
    );
    return true;
}

} // namespace slotv2::progress_event
