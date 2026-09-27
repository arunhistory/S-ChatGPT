#include "section/section_transition.hpp"

namespace slotv2::section_transition {

Result applyPendingTierUp(
    machine_state::State& machine,
    pending_event::State& pending
) {
    Result out{};
    out.before = machine.at.tier;
    out.after = machine.at.tier;

    if (!pending_event::has(pending, pending_event::SectionTierUp)
        || machine.area != machine_state::Area::AT
        || !machine.at.active) {
        return out;
    }

    if (machine.at.tier == at_state::Tier::Lower) {
        at_state::setTier(machine.at, at_state::Tier::Middle);
        out.tier_changed = true;
    } else if (machine.at.tier == at_state::Tier::Middle) {
        at_state::setTier(machine.at, at_state::Tier::Upper);
        out.tier_changed = true;
    }

    // Even if already Upper, the queued reward has been resolved and must
    // not remain as a permanent stale pending bit.
    (void)pending_event::consume(
        pending,
        pending_event::SectionTierUp
    );
    out.applied = true;
    out.after = machine.at.tier;
    return out;
}

Result apply(
    machine_state::State& machine,
    pending_event::State& pending,
    const section_flow::Result& flow
) {
    Result out{};
    out.before = machine.at.tier;
    out.after = machine.at.tier;

    if (!flow.cut) return out;

    switch (flow.reward.kind) {
        case section_reward::Kind::TierUp:
            return applyPendingTierUp(machine, pending);

        case section_reward::Kind::Special:
            if (machine.area != machine_state::Area::AT
                || !machine.at.active
                || machine.at.tier != at_state::Tier::Upper
                || machine.chain_zone.active
                || machine.special_zone.active
                || machine.upper_special.active
                || machine.entry_gate.active) {
                // Keep SectionSpecial pending for the runtime deferred route.
                return out;
            }

            special_zone::start(machine.special_zone);
            out.applied = true;
            out.special_started = true;
            (void)pending_event::consume(
                pending,
                pending_event::SectionSpecial
            );
            return out;

        case section_reward::Kind::UpperSpecial:
            if (machine.area != machine_state::Area::AT
                || !machine.at.active
                || machine.at.tier != at_state::Tier::Upper) {
                return out;
            }

            out.upper_special_pending = true;
            return out;

        case section_reward::Kind::None:
        default:
            return out;
    }
}

} // namespace slotv2::section_transition
