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
enum class Route : uint8_t { Direct = 0, Omen = 1, OmenCZ = 2 };
enum class Completion : uint8_t { None = 0, GateReady = 1, CZStarted = 2 };

struct State {
    Stage stage{Stage::None};
    Route route{Route::Direct};
    uint8_t omen_games_left{0};
    uint8_t omen_games_total{0};
    // The award, including the selected BONUS/AT kind, tier, stock and
    // return target, is fixed before any omen or optional CZ begins.
    entry_gate::State awarded_gate{};
};

struct Capture {
    bool captured{false};
    Route route{Route::Direct};
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

// For non-debug draws, exactly 1% of normal awards are direct.
// The other 99% use a 10-25 G omen. Among omen routes, 20% additionally
// lead into a guaranteed-win CZ; it presents the previously fixed award.
// The 25 G limit refers to the omen, not the subsequent CZ/start-symbol wait.
inline Capture capture(
    Rng& rng,
    State& state,
    entry_gate::State& gate,
    bool allow_cz = true,
    uint8_t debug_route = kRandomRoute
) {
    if (active(state) || !gate.active) return {};

    Route route = Route::Direct;
    if (debug_route <= static_cast<uint8_t>(Route::OmenCZ)) {
        route = static_cast<Route>(debug_route);
    } else if (!rng.oneIn(100u)) {
        route = allow_cz && rng.uniformBelow(5u) == 0u
            ? Route::OmenCZ
            : Route::Omen;
    }
    // A CZ is already completed if this award came out of a real CZ.
    if (route == Route::OmenCZ && !allow_cz) route = Route::Omen;

    if (route == Route::Direct) {
        return {true, Route::Direct, 0u};
    }

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
inline Completion completeOmenGame(
    State& state,
    entry_gate::State& gate
) {
    if (!inOmen(state) || state.omen_games_left == 0u) {
        return Completion::None;
    }
    --state.omen_games_left;
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
