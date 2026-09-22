#pragma once
#include <stdint.h>
#include "../shared/rng.hpp"
#include "../bonus-state/index.hpp"

namespace slotv2::bonus_cycle {

enum class Outcome : uint8_t {
    None = 0,
    Completed = 1,
    EpisodeUpgradePending = 2
};

struct Result {
    bool active_before{false};
    bonus_state::Kind kind{bonus_state::Kind::Regular};
    int medals_before{0};
    int medals_after{0};
    Outcome outcome{Outcome::None};
};

// BONUSの純増分を反映する。
// 通常50枚 / Episode80枚の目標到達を判定。
// 通常BONUS完了時だけ1%のEpisode昇格を抽選するが、
// 実際の次状態遷移はruntime側でpendingとして扱う。
Result applyNetGain(
    Rng& rng,
    bonus_state::State& state,
    int net_gain
);

} // namespace slotv2::bonus_cycle
