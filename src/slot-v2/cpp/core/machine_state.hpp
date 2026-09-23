#pragma once
#include <stdint.h>
#include "../section/index.hpp"
#include "../stock/index.hpp"
#include "../normal-progress/index.hpp"
#include "../normal-state/index.hpp"
#include "../normal-high/index.hpp"
#include "../at-state/index.hpp"
#include "../at-omen/index.hpp"
#include "../bonus-state/index.hpp"
#include "../cz-state/index.hpp"
#include "../special-zone/index.hpp"
#include "../upper-special/index.hpp"
#include "entry/entry_gate.hpp"
#include "../upper-comeback/index.hpp"
#include "../revival-state/index.hpp"

namespace slotv2::machine_state {

enum class Area : uint8_t {
    Normal = 0,
    CZ = 1,
    Bonus = 2,
    AT = 3,
    Revival = 4
};

struct State {
    Area area{Area::Normal};
    section::State section{};
    normal_state::State normal{};
    normal_high::State normal_high{};
    stock::State stock{};
    normal_progress::State normal_progress{};
    at_state::State at{};
    at_omen::State at_omen{};
    bonus_state::State bonus{};
    Area bonus_return_area{Area::Normal};
    bool bonus_return_valid{false};
    cz_state::State cz{};
    special_zone::State special_zone{};
    upper_special::State upper_special{};
    entry_gate::State entry_gate{};
    upper_comeback::State upper_comeback{};
    revival_state::State revival{};
};

void reset(State& state);

} // namespace slotv2::machine_state
