#pragma once
#include <stdint.h>

namespace slotv2::stock {

struct State {
    uint32_t count{0};
};

void add(State& state, uint32_t amount = 1);
bool consumeOne(State& state);
uint8_t preferenceLevel(const State& state);

// 有利区間切断時は、保有数そのものを次区間へ持ち越さず
// 0/1/3/5段階の優遇レベルへ変換してからクリアする。
uint8_t consumeForSection(State& state);

} // namespace slotv2::stock
