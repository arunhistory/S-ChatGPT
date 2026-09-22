#include "index.hpp"
#include "../progress-event/index.hpp"

namespace slotv2::normal_flow_transition {

Result apply(
    machine_state::State& machine,
    pending_event::State& pending,
    normal_mode::Mode mode,
    normal_flow::Reward reward
) {
    if (machine.area != machine_state::Area::Normal) return {};

    Result out{};

    if (reward == normal_flow::Reward::CZ) {
        cz_state::start(machine.cz);
        machine.area = machine_state::Area::CZ;
        out.outcome = Outcome::CZStarted;
        return out;
    }

    if (reward == normal_flow::Reward::Bonus) {
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
            out.outcome = Outcome::ATQueued;
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

    if (reward == normal_flow::Reward::LowerAT) {
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
            out.outcome = Outcome::ATQueued;
        }
    }

    return out;
}

} // namespace slotv2::normal_flow_transition
