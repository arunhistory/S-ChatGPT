#include "index.hpp"
#include "../bonus-transition/index.hpp"

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

    bonus_transition::start(
        machine,
        policy.kind,
        machine_state::Area::Normal
    );

    return {
        Outcome::Bonus,
        policy.kind
    };
}

Result enterAT(machine_state::State& machine) {
    if (machine.area != machine_state::Area::Normal) return {};

    at_state::start(
        machine.at,
        at_state::Tier::Lower
    );
    machine.area = machine_state::Area::AT;

    return {
        Outcome::LowerAT,
        bonus_state::Kind::Regular
    };
}

} // namespace slotv2::normal_hit_entry
