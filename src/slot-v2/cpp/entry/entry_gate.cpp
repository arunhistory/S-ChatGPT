#include "entry/entry_gate.hpp"

namespace slotv2::entry_gate {

void clear(State& state) {
    state = {};
}

void queueBonus(
    State& state,
    bonus_state::Kind kind,
    bool return_to_at
) {
    state = {};
    state.active = true;
    state.kind = Kind::Bonus;
    state.bonus_kind = kind;
    state.bonus_return_to_at = return_to_at;
}

void queueAT(
    State& state,
    at_state::Tier tier,
    uint32_t stock_to_add
) {
    state = {};
    state.active = true;
    state.kind = Kind::AT;
    state.at_tier = tier;
    state.stock_to_add = stock_to_add;
}

bool beginGame(State& state, Rng& rng) {
    if (!state.active) return false;
    state.armed_this_game = rng.oneIn(2u);
    return state.armed_this_game;
}

RoleFlag roleForArmed(const State& state) {
    if (!state.active || !state.armed_this_game) return RoleFlag::Miss;

    switch (state.kind) {
        case Kind::Bonus:
            return RoleFlag::EntryBonus;
        case Kind::AT:
            return RoleFlag::EntryAT;
        case Kind::None:
        default:
            return RoleFlag::Miss;
    }
}

bool matches(
    const State& state,
    Symbol left,
    Symbol middle,
    Symbol right
) {
    if (!state.active || !state.armed_this_game) return false;

    if (state.kind == Kind::AT) {
        return left == Symbol::Red7
            && middle == Symbol::Red7
            && right == Symbol::Red7;
    }

    if (state.kind == Kind::Bonus) {
        return left == Symbol::Red7
            && middle == Symbol::Red7
            && right == Symbol::Bar;
    }

    return false;
}

} // namespace slotv2::entry_gate
