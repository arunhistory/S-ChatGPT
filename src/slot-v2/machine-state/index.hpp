#pragma once
#include <stdint.h>
#include "../section/index.hpp"
#include "../stock/index.hpp"
#include "../normal-progress/index.hpp"
#include "../at-state/index.hpp"
#include "../bonus-state/index.hpp"
#include "../cz-state/index.hpp"

namespace slotv2::machine_state {

enum class Area : uint8_t {
    Normal = 0,
    CZ = 1,
    Bonus = 2,
    AT = 3
};

struct State {
    Area area{Area::Normal};
    section::State section{};
    stock::State stock{};
    normal_progress::State normal_progress{};
    at_state::State at{};
    bonus_state::State bonus{};
    cz_state::State cz{};
};

void reset(State& state);

} // namespace slotv2::machine_state
