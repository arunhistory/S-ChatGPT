#include "normal/normal_ceiling_transition.hpp"
#include "core/progress_event.hpp"

namespace slotv2::normal_ceiling_transition {

Result apply(
    machine_state::State& machine,
    pending_event::State& pending,
    normal_mode::Mode mode,
    normal_ceiling::Reward reward
) {
    if (machine.area != machine_state::Area::Normal) return {};

    Result out{};

    switch (reward) {
        case normal_ceiling::Reward::CZ:
            cz_state::start(machine.cz);
            machine.area = machine_state::Area::CZ;
            out.outcome = Outcome::CZStarted;
            return out;

        case normal_ceiling::Reward::Bonus:
            out.trigger = normal_at_trigger::applyNextHitGuarantee(
                machine,
                pending,
                false
            );

            if (out.trigger.started) {
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

        case normal_ceiling::Reward::LowerAT:
            out.trigger = normal_at_trigger::applyNextHitGuarantee(
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
            return out;

        case normal_ceiling::Reward::LowerATWithStock:
            // Special 777G: lower AT + one stock.
            // The stock uses the same restart-game lottery as an ordinary stock.
            (void)normal_at_trigger::applyNextHitGuarantee(
                machine,
                pending,
                true
            );
            entry_gate::queueAT(
                machine.entry_gate,
                at_state::Tier::Lower,
                1u
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
            out.outcome = Outcome::ATWithStockQueued;
            return out;

        case normal_ceiling::Reward::Freeze:
            // Freeze itself is performed on the next game by the existing
            // forced freeze/special control path.
            out.outcome = Outcome::FreezeQueued;
            return out;

        case normal_ceiling::Reward::None:
        default:
            return out;
    }
}

} // namespace slotv2::normal_ceiling_transition
