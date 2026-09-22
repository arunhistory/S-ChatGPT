#include "index.hpp"

namespace slotv2::section_transition {

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
            if (machine.area != machine_state::Area::AT || !machine.at.active) {
                return out;
            }

            if (machine.at.tier == at_state::Tier::Lower) {
                at_state::setTier(machine.at, at_state::Tier::Middle);
                out.tier_changed = true;
            } else if (machine.at.tier == at_state::Tier::Middle) {
                at_state::setTier(machine.at, at_state::Tier::Upper);
                out.tier_changed = true;
            }

            out.applied = out.tier_changed;
            out.after = machine.at.tier;

            if (out.applied) {
                (void)pending_event::consume(
                    pending,
                    pending_event::SectionTierUp
                );
            }
            return out;

        case section_reward::Kind::Special:
            special_zone::start(machine.special_zone);
            out.applied = true;
            out.special_started = true;
            (void)pending_event::consume(
                pending,
                pending_event::SectionSpecial
            );
            return out;

        case section_reward::Kind::UpperSpecial:
            out.upper_special_pending = true;
            return out;

        case section_reward::Kind::None:
        default:
            return out;
    }
}

} // namespace slotv2::section_transition
