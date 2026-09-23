#include "index.hpp"
#include "../revival-lottery/index.hpp"
#include "../stock/index.hpp"

namespace slotv2::revival_cycle {

Game beginGame(
    Rng& rng,
    revival_state::State& state,
    RoleFlag role
) {
    if (!state.active || state.games_left == 0u) return {};

    Game out{};
    out.active = true;
    out.role = role;
    out.games_before = state.games_left;
    out.revival_hit = revival_lottery::draw(rng, role);

    --state.games_left;
    out.games_after = state.games_left;
    return out;
}

FinalizeResult finalizeGame(
    machine_state::State& machine,
    revival_state::State& state,
    const Game& game
) {
    if (!game.active || !state.active) return {};

    if (game.revival_hit) {
        const auto tier = state.revive_tier;
        const bool add_stock = state.kicked_normal_hit;

        state.active = false;
        state.games_left = 0u;
        state.kicked_normal_hit = false;

        at_state::start(machine.at, tier);
        machine.area = machine_state::Area::AT;

        if (add_stock) {
            stock::add(machine.stock, 1u);
        }

        return {
            Outcome::Revived,
            tier,
            add_stock
        };
    }

    if (state.games_left == 0u) {
        const auto tier = state.revive_tier;
        revival_state::clear(state);
        machine.area = machine_state::Area::Normal;

        return {
            Outcome::Failed,
            tier,
            false
        };
    }

    return {
        Outcome::Running,
        state.revive_tier,
        false
    };
}

} // namespace slotv2::revival_cycle
