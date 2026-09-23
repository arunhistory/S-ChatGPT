#pragma once
#include <stdint.h>
#include "../machine-state/index.hpp"

namespace slotv2::at_window {

enum class Status : uint8_t {
    NotApplicable = 0,
    Running = 1,
    EmptyNoStock = 2,
    EmptyStockAvailable = 3
};

struct Result {
    Status status{Status::NotApplicable};
    uint32_t stock_count{0};
};

// AT残Gが0になった時点で、ストックがあるかだけ判定する。
// ストック消費・再開G数・中位/上位の継続/終了はここでは決めない。
Result inspect(const machine_state::State& machine);

} // namespace slotv2::at_window
