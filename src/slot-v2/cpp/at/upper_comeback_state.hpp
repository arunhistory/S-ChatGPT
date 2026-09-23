#pragma once
#include <stdint.h>
#include "shared/rng.hpp"

namespace slotv2::upper_comeback {

static constexpr uint8_t kWindowGames = 64;

struct State {
    bool active{false};
    uint8_t games_left{0};
};

void start(State& state);
bool consumeGame(State& state);

// 64G窓終了時の20%引戻し判定。
// 窓内の演出・前兆配置は別プログラムに持たせる。
bool judge(Rng& rng);

} // namespace slotv2::upper_comeback
