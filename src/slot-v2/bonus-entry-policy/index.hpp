#pragma once
#include "../normal-mode/index.hpp"
#include "../bonus-state/index.hpp"

namespace slotv2::bonus_entry_policy {

enum class Status : uint8_t {
    Resolved = 0,
    SpecialModeUnresolved = 1
};

struct Result {
    Status status{Status::Resolved};
    bonus_state::Kind kind{bonus_state::Kind::Regular};
};

// 天国/超天国はエピソード直当たり。
// 通常A/Bは通常BONUS。
// 特殊モードのBONUS種別はここでは決めない。
Result forMode(normal_mode::Mode mode);

} // namespace slotv2::bonus_entry_policy
