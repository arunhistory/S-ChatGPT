#include "index.hpp"

namespace slotv2::normal_hit_entry {

Result enterBonus(
    machine_state::State& machine,
    normal_mode::Mode mode
) {
    if (machine.area != machine_state::Area::Normal) return {};

    const auto policy = bonus_entry_policy::forMode(mode);
    if (policy.status != bonus_entry_policy::Status::Resolved) {
        return {
            Outcome::UnresolvedSpecialMode,
            policy.kind
        };
    }

    entry_gate::queueBonus(
        machine.entry_gate,
        policy.kind,
        false
    );

    return {
        Outcome::Bonus,
        policy.kind
    };
}

Result enterAT(machine_state::State& machine) {
    if (machine.area != machine_state::Area::Normal) return {};

    entry_gate::queueAT(
        machine.entry_gate,
        at_state::Tier::Lower
    );

    return {
        Outcome::LowerAT,
        bonus_state::Kind::Regular
    };
}

} // namespace slotv2::normal_hit_entry
