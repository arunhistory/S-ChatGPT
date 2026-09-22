#pragma once
#include "../machine-state/index.hpp"
#include "../normal-mode/index.hpp"
#include "../bonus-entry-policy/index.hpp"

namespace slotv2::normal_hit_entry {

enum class Outcome : uint8_t {
    None = 0,
    Bonus = 1,
    LowerAT = 2,
    UnresolvedSpecialMode = 3
};

struct Result {
    Outcome outcome{Outcome::None};
    bonus_state::Kind bonus_kind{bonus_state::Kind::Regular};
};

// 「通常当たりがBONUSだった」と確定した後の入口。
// NormalA/B=通常BONUS、天国/超天国=Episode。
// Special modeだけは仕様未確定なので遷移しない。
Result enterBonus(
    machine_state::State& machine,
    normal_mode::Mode mode
);

// 「通常当たりがATだった」と確定した後の入口。
// 通常ATは下位100Gから開始。
Result enterAT(machine_state::State& machine);

} // namespace slotv2::normal_hit_entry
