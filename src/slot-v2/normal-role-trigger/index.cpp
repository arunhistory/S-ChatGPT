#include "index.hpp"
#include "../progress-event/index.hpp"

namespace slotv2::normal_role_trigger {

DrawResult draw(
    Rng& rng,
    RoleFlag role
) {
    if (role == RoleFlag::StrongCherry) {
        // 50% BONUS. Remaining 50% is AT:
        // lower 2/3, middle 1/3.
        if (rng.uniformBelow(2u) == 0u) {
            return DrawResult::Bonus;
        }

        return rng.uniformBelow(3u) < 2u
            ? DrawResult::LowerAT
            : DrawResult::MiddleAT;
    }

    if (role == RoleFlag::StrongChance) {
        return rng.uniformBelow(100u) == 0u
            ? DrawResult::LowerAT
            : DrawResult::None;
    }

    return DrawResult::None;
}

ApplyResult apply(
    machine_state::State& machine,
    pending_event::State& pending,
    normal_mode::Mode mode,
    DrawResult result
) {
    if (machine.area != machine_state::Area::Normal) return {};

    ApplyResult out{};

    if (result == DrawResult::Bonus) {
        out.guarantee = normal_at_trigger::applyNextHitGuarantee(
            machine,
            pending,
            false
        );

        if (out.guarantee.started) {
            progress_event::onNormalHitResolved(
                machine.normal_progress,
                pending,
                true
            );
            out.entry = {
                normal_hit_entry::Outcome::LowerAT,
                bonus_state::Kind::Regular
            };
            out.outcome = Outcome::LowerATQueued;
            return out;
        }

        out.entry = normal_hit_entry::enterBonus(
            machine,
            mode
        );

        if (out.entry.outcome == normal_hit_entry::Outcome::Bonus) {
            progress_event::onNormalHitResolved(
                machine.normal_progress,
                pending,
                false
            );
            out.outcome = Outcome::BonusQueued;
        }
        return out;
    }

    if (result == DrawResult::LowerAT) {
        out.guarantee = normal_at_trigger::applyNextHitGuarantee(
            machine,
            pending,
            true
        );
        out.entry = normal_hit_entry::enterAT(machine);

        if (out.entry.outcome == normal_hit_entry::Outcome::LowerAT) {
            progress_event::onNormalHitResolved(
                machine.normal_progress,
                pending,
                true
            );
            out.outcome = Outcome::LowerATQueued;
        }
        return out;
    }

    if (result == DrawResult::MiddleAT) {
        out.guarantee = normal_at_trigger::applyNextHitGuarantee(
            machine,
            pending,
            true
        );

        entry_gate::queueAT(
            machine.entry_gate,
            at_state::Tier::Middle
        );

        progress_event::onNormalHitResolved(
            machine.normal_progress,
            pending,
            true
        );

        out.entry = {
            normal_hit_entry::Outcome::LowerAT,
            bonus_state::Kind::Regular
        };
        out.outcome = Outcome::MiddleATQueued;
        return out;
    }

    return out;
}

} // namespace slotv2::normal_role_trigger
