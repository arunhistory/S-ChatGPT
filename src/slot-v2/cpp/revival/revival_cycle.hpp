#pragma once
#include <stdint.h>
#include "shared/rng.hpp"
#include "shared/types.hpp"
#include "core/machine_state.hpp"
#include "revival/revival_state.hpp"

namespace slotv2::revival_cycle {

enum class Outcome : uint8_t {
    None = 0,
    Running = 1,
    Revived = 2,
    Failed = 3
};

struct Game {
    bool active{false};
    RoleFlag role{RoleFlag::None};
    uint8_t games_before{0};
    uint8_t games_after{0};
    bool revival_hit{false};
};

struct FinalizeResult {
    Outcome outcome{Outcome::None};
    at_state::Tier tier{at_state::Tier::Lower};
    bool stock_added{false};
};

// レバー時に成立役に応じた復活抽選を行い、5G窓を1G消費。
Game beginGame(
    Rng& rng,
    revival_state::State& state,
    RoleFlag role
);

// 3停止完了後に復活/失敗を確定。
// 復活成功時は終了したATと同Tierを初回100Gとして復活。
// 5G中に通常当たり枠が1回以上蹴られていればストック+1。
FinalizeResult finalizeGame(
    machine_state::State& machine,
    revival_state::State& state,
    const Game& game
);

} // namespace slotv2::revival_cycle
