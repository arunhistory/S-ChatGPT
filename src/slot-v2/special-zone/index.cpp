#include "index.hpp"

namespace slotv2::special_zone {

void start(State& state) {
    state.active = true;
    state.games_left = kGames;
}

HitResult playOne(State& state, Rng& rng) {
    if (!state.active || state.games_left == 0u) return HitResult::None;

    --state.games_left;
    if (state.games_left == 0u) state.active = false;

    // 各G 1/2で当たり。成立時95%追加G / 5%当たり。
    if (!rng.oneIn(2u)) return HitResult::None;
    return rng.oneIn(20u) ? HitResult::Bonus : HitResult::AddGames;
}

} // namespace slotv2::special_zone
