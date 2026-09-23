#include "bonus/bonus_transition.hpp"

namespace slotv2::bonus_transition {

void start(
    machine_state::State& machine,
    bonus_state::Kind kind,
    machine_state::Area return_area
) {
    machine.bonus_return_area = return_area;
    machine.bonus_return_valid = true;
    bonus_state::start(machine.bonus, kind);
    machine.area = machine_state::Area::Bonus;
}

Result finalize(
    machine_state::State& machine,
    pending_event::State& pending,
    const bonus_cycle::Result& cycle
) {
    if (cycle.outcome == bonus_cycle::Outcome::None) return {};

    if (cycle.outcome == bonus_cycle::Outcome::EpisodeUpgradePending) {
        const auto return_area = machine.bonus_return_valid
            ? machine.bonus_return_area
            : machine_state::Area::Normal;

        bonus_state::start(
            machine.bonus,
            bonus_state::Kind::Episode
        );
        machine.area = machine_state::Area::Bonus;

        (void)pending_event::consume(
            pending,
            pending_event::BonusEpisodeUpgrade
        );

        return {
            Outcome::EpisodeStarted,
            return_area
        };
    }

    const auto return_area = machine.bonus_return_valid
        ? machine.bonus_return_area
        : machine_state::Area::Normal;

    machine.area = return_area;
    machine.bonus_return_valid = false;

    (void)pending_event::consume(
        pending,
        pending_event::BonusComplete
    );

    return {
        Outcome::Returned,
        return_area
    };
}

} // namespace slotv2::bonus_transition
