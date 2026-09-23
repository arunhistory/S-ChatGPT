#include "at/at_internal_transition.hpp"
#include "at/at_add_games.hpp"

namespace slotv2::at_internal_transition {

Result apply(
    Rng& rng,
    machine_state::State& machine,
    pending_event::State& pending
) {
    Result out{};
    out.before = machine.at.tier;
    out.after = machine.at.tier;

    if (machine.area != machine_state::Area::AT || !machine.at.active) {
        return out;
    }

    if (pending_event::has(pending, pending_event::ATAddGames)) {
        const uint16_t games = at_add_games::draw(rng);
        at_state::addGames(machine.at, static_cast<int>(games));
        (void)pending_event::consume(pending, pending_event::ATAddGames);

        out.applied = true;
        out.add_games_applied = true;
        out.added_games = games;
        return out;
    }

    if (!pending_event::has(pending, pending_event::ATFall)) {
        return out;
    }

    if (machine.at.tier == at_state::Tier::Lower) {
        // A lower Fall must have been armed on the Fall game's lever-on.
        // If that invariant is broken, preserve ATFall pending and do not
        // invent a fallback termination rule.
        if (!lower_fall_challenge::beginWaiting(
                machine.lower_fall_challenge
            )) {
            return out;
        }

        (void)pending_event::consume(
            pending,
            pending_event::ATFall
        );
        out.applied = true;
        out.fall_applied = true;
        out.lower_fall_challenge_started = true;
        return out;
    }

    (void)pending_event::consume(pending, pending_event::ATFall);
    out.applied = true;
    out.fall_applied = true;

    if (machine.at.tier == at_state::Tier::Middle) {
        at_state::setTier(machine.at, at_state::Tier::Lower);
        out.after = at_state::Tier::Lower;
        return out;
    }

    // Upper Fall terminates the current AT set.
    // Keep AT active at 0G so the shared AT-window resolver can apply
    // stock / upper comeback / revival in the correct order.
    machine.at.games_left = 0;
    pending_event::add(pending, pending_event::ATWindowEmpty);
    out.at_end_pending = true;
    return out;
}

} // namespace slotv2::at_internal_transition
