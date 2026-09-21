#pragma once
#include <stdint.h>

namespace slotv2::normal_progress {

struct State {
    uint8_t cz_misses{0};
    uint8_t normal_hits_without_at{0};
    uint8_t bell9_streak{0};
    bool next_hit_at_guaranteed{false};
};

// 9枚ベルが5連続した瞬間だけ true。
bool onBell9(State& state);
void onNonBell9(State& state);

// CZ3スルー天井。3回目の失敗で true を返してカウンタをクリア。
bool onCZResult(State& state, bool hit);

// 通常当たりがATではなかった場合の連続回数を管理。
// 5連続到達で「次回AT保証」を立てる。
void onNormalHit(State& state, bool was_at);
bool consumeNextATGuarantee(State& state);

} // namespace slotv2::normal_progress
