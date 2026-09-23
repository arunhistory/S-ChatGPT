#include "index.hpp"
#include "../normal-at-trigger/index.hpp"
#include "../normal-hit-entry/index.hpp"
#include "../progress-event/index.hpp"

namespace slotv2::cz_reward {

Target targetFromRoll(uint16_t roll_0_to_9) {
    return (roll_0_to_9 % 10u) < 3u
        ? Target::LowerAT
        : Target::NormalHit;
}

Target drawTarget(Rng& rng) {
    return targetFromRoll(
        static_cast<uint16_t>(rng.uniformBelow(10u))
    );
}

Result apply(
    Rng& rng,
    machine_state::State& machine,
    pending_event::State& pending,
    normal_mode::Mode mode
) {
    const bool hit = pending_event::has(pending, pending_event::CZHit);
    const bool three_miss = pending_event::has(
        pending,
        pending_event::CZThreeMissHit
    );

    if (!hit && !three_miss) return {};

    (void)pending_event::consume(pending, pending_event::CZHit);
    (void)pending_event::consume(pending, pending_event::CZThreeMissHit);

    // CZ is finished before the reward is routed.
    machine.area = machine_state::Area::Normal;

    const auto target = drawTarget(rng);

    if (target == Target::LowerAT) {
        const auto guarantee = normal_at_trigger::applyNextHitGuarantee(
            machine,
            pending,
            true
        );

        const auto entry = normal_hit_entry::enterAT(machine);
        if (entry.outcome == normal_hit_entry::Outcome::LowerAT) {
            progress_event::onNormalHitResolved(
                machine.normal_progress,
                pending,
                true
            );
        }

        return {
            Outcome::LowerAT,
            three_miss,
            guarantee.guarantee_consumed
        };
    }

    const auto guarantee = normal_at_trigger::applyNextHitGuarantee(
        machine,
        pending,
        false
    );

    if (guarantee.started) {
        progress_event::onNormalHitResolved(
            machine.normal_progress,
            pending,
            true
        );

        return {
            Outcome::LowerAT,
            three_miss,
            guarantee.guarantee_consumed
        };
    }

    const auto entry = normal_hit_entry::enterBonus(machine, mode);
    if (entry.outcome == normal_hit_entry::Outcome::Bonus) {
        progress_event::onNormalHitResolved(
            machine.normal_progress,
            pending,
            false
        );

        return {
            Outcome::NormalHit,
            three_miss,
            false
        };
    }

    return {};
}

} // namespace slotv2::cz_reward
