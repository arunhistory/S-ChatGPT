#pragma once
#include <stdint.h>
#include "shared/rng.hpp"
#include "entry/entry_gate.hpp"

namespace slotv2::normal_latent {

// These timings apply to already-awarded NORMAL hits only. Independent
// special-direct draws (1/8192, 1/32768, freeze) keep their existing path.
static constexpr uint8_t kMinimumOmenGames = 10u;
static constexpr uint8_t kMaximumOmenGames = 25u;
static constexpr uint8_t kRandomRoute = 0xffu;

enum class Stage : uint8_t { None = 0, Omen = 1, ScriptedCZ = 2 };
enum class Route : uint8_t { NormalEvent = 0, Omen = 1, OmenCZ = 2 };
enum class Completion : uint8_t {
    None = 0, GateReady = 1, CZStarted = 2, NormalEventReady = 3
};

struct State {
    Stage stage{Stage::None};
    Route route{Route::NormalEvent};
    uint8_t omen_games_left{0};
    uint8_t omen_games_total{0};
    // The award, including the selected BONUS/AT kind, tier, stock and
    // return target, is fixed before any omen or optional CZ begins.
    entry_gate::State awarded_gate{};
};

struct Capture {
    bool captured{false};
    Route route{Route::NormalEvent};
    uint8_t omen_games{0};
};

inline bool inOmen(const State& state) {
    return state.stage == Stage::Omen;
}
inline bool inCZ(const State& state) {
    return state.stage == Stage::ScriptedCZ;
}
inline bool active(const State& state) {
    return state.stage != Stage::None;
}

// Ordinary normal hit presentation, determined when the award is fixed:
// 95%: 10-25G omen -> a scripted, guaranteed-winning CZ.
//  4%: 10-25G omen -> ordinary announcement without CZ.
//  1%: 10-25G concealed wait -> reveal via an ordinary normal event.
// NONE of these routes reveal the reward on the next spin.
// An award arising from a REAL successful CZ does not enter a second CZ.
// Omen length excludes the subsequent CZ and start-symbol waiting games.
inline Capture capture(
    Rng& rng,
    State& state,
    entry_gate::State& gate,
    bool allow_cz = true,
    uint8_t debug_route = kRandomRoute
) {
    if (active(state) || !gate.active) return {};

    Route route = Route::OmenCZ;
    if (debug_route <= static_cast<uint8_t>(Route::OmenCZ)) {
        route = static_cast<Route>(debug_route);
    } else {
        const auto roll = rng.uniformBelow(100u);
        route = roll < 95u ? Route::OmenCZ
            : (roll < 99u ? Route::Omen : Route::NormalEvent);
    }
    // An award that came from a real CZ must not repeat its CZ.
    if (route == Route::OmenCZ && !allow_cz) route = Route::Omen;

    state = {};
    state.stage = Stage::Omen;
    state.route = route;
    state.omen_games_total = static_cast<uint8_t>(
        kMinimumOmenGames + rng.uniformBelow(
            static_cast<uint32_t>(
                kMaximumOmenGames - kMinimumOmenGames + 1u
            )
        )
    );
    state.omen_games_left = state.omen_games_total;
    state.awarded_gate = gate;
    entry_gate::clear(gate);
    return {true, route, state.omen_games_total};
}

// Called ONLY after each completed three-reel omen game. Lever-on alone
// cannot shorten the presentation. No lottery can replace the held award.
inline bool ordinaryRevealEvent(RoleFlag role) {
    switch (role) {
        case RoleFlag::WeakCherry:
        case RoleFlag::StrongCherry:
        case RoleFlag::Watermelon:
        case RoleFlag::WeakChance:
        case RoleFlag::StrongChance:
        case RoleFlag::PenguinChance:
            return true;
        default:
            return false;
    }
}

// Called only after the third stop of a concealed normal-play game.
// On the 1% route, reveal on the first real normal event after at least
// ten completed games, or emit a presentation-only event at the selected
// 10-25G deadline. Neither outcome redraws or changes the fixed award
// and the physical lottery is never forced.
inline Completion completeOmenGame(
    State& state,
    entry_gate::State& gate,
    RoleFlag natural_role = RoleFlag::None
) {
    if (!inOmen(state) || state.omen_games_left == 0u) {
        return Completion::None;
    }
    --state.omen_games_left;
    const uint8_t elapsed = static_cast<uint8_t>(
        state.omen_games_total - state.omen_games_left
    );
    if (state.route == Route::NormalEvent) {
        if (elapsed < kMinimumOmenGames ||
            (!ordinaryRevealEvent(natural_role) &&
                state.omen_games_left > 0u)) {
            return Completion::None;
        }
        gate = state.awarded_gate;
        state = {};
        return Completion::NormalEventReady;
    }
    if (state.omen_games_left > 0u) return Completion::None;
    if (state.route == Route::OmenCZ) {
        state.stage = Stage::ScriptedCZ;
        return Completion::CZStarted;
    }
    gate = state.awarded_gate;
    state = {};
    return Completion::GateReady;
}

// Scripted CZ must resolve the internally awarded hit, not draw a new
// BONUS/AT target. Its own role-based CZ success can reveal early; failure
// on the last CZ game is converted to an eventual confirmed success.
inline bool completeScriptedCZ(
    State& state,
    entry_gate::State& gate
) {
    if (!inCZ(state) || gate.active || !state.awarded_gate.active) {
        return false;
    }
    gate = state.awarded_gate;
    state = {};
    return true;
}

} // namespace slotv2::normal_latent
