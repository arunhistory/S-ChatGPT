#pragma once
#include "../machine-state/index.hpp"
#include "../pending-event/index.hpp"
#include "../bonus-cycle/index.hpp"

namespace slotv2::bonus_transition {

enum class Outcome : uint8_t {
    None = 0,
    EpisodeStarted = 1,
    Returned = 2
};

struct Result {
    Outcome outcome{Outcome::None};
    machine_state::Area return_area{machine_state::Area::Normal};
};

// BONUS開始時に復帰先を保存する。
void start(
    machine_state::State& machine,
    bonus_state::Kind kind,
    machine_state::Area return_area
);

// BONUS完了結果を機械状態へ反映。
// 通常BONUSの1%昇格ならEpisodeを即開始。
// 完了なら保存していた復帰先へ戻る。
Result finalize(
    machine_state::State& machine,
    pending_event::State& pending,
    const bonus_cycle::Result& cycle
);

} // namespace slotv2::bonus_transition
