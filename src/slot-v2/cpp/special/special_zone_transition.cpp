#include "special/special_zone_transition.hpp"

namespace slotv2::special_zone_transition {

Result apply(
    machine_state::State& machine,
    pending_event::State& pending
) {
    if (pending_event::has(pending, pending_event::SpecialZoneAddGames)) {
        if (machine.area != machine_state::Area::AT || !machine.at.active) {
            return {};
        }

        const uint16_t games = machine.special_zone.pending_add_games;
        if (games == 0u) return {};

        at_state::addGames(
            machine.at,
            static_cast<int>(games)
        );
        machine.special_zone.pending_add_games = 0u;

        (void)pending_event::consume(
            pending,
            pending_event::SpecialZoneAddGames
        );

        return {
            Outcome::AddGamesApplied,
            games
        };
    }

    if (pending_event::has(pending, pending_event::SpecialZoneBonus)) {
        if (machine.area != machine_state::Area::AT || !machine.at.active) {
            return {};
        }

        if (machine.at_omen.active) return {};

        at_omen::start(
            machine.at_omen,
            false
        );

        machine.special_zone.pending_add_games = 0u;

        (void)pending_event::consume(
            pending,
            pending_event::SpecialZoneBonus
        );

        return {
            Outcome::BonusOmenStarted,
            0u
        };
    }

    return {};
}

} // namespace slotv2::special_zone_transition
