#pragma once
#include <stdint.h>
#include "../shared/types.hpp"

namespace slotv2::acquisition {

enum class Status : uint8_t {
    NotReady = 0,
    Acquired = 1,
    Missed = 2,
    NoPayoutRole = 3,
    Substitute = 4
};

struct Result {
    RoleFlag internal_role{RoleFlag::None};
    Status status{Status::NotReady};
    int medals{0};
};

// 内部成立役は変更しない。
// 実停止3リールの中段1ラインだけを見て「取れた/取りこぼした」を返す。
Result judge(
    RoleFlag internal_role,
    uint8_t left_pos,
    uint8_t middle_pos,
    uint8_t right_pos,
    bool had_substitute,
    bool had_role_miss
);

} // namespace slotv2::acquisition
