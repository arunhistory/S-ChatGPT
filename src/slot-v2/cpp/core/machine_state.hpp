#pragma once
#include <stdint.h>
#include "section/section_state.hpp"
#include "stock/stock_state.hpp"
#include "normal/normal_progress.hpp"
#include "normal/normal_state.hpp"
#include "normal/normal_high.hpp"
#include "at/at_state.hpp"
#include "at/at_omen.hpp"
#include "at/lower_fall_challenge.hpp"
#include "bonus/bonus_state.hpp"
#include "cz/cz_state.hpp"
#include "special/special_zone.hpp"
#include "special/chain_zone.hpp"
#include "special/upper_special.hpp"
#include "entry/entry_gate.hpp"
#include "at/upper_comeback_state.hpp"
#include "revival/revival_state.hpp"

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
    lower_fall_challenge::State lower_fall_challenge{};
    bonus_state::State bonus{};
    Area bonus_return_area{Area::Normal};
    bool bonus_return_valid{false};
    cz_state::State cz{};
    special_zone::State special_zone{};
    chain_zone::State chain_zone{};
    upper_special::State upper_special{};
    entry_gate::State entry_gate{};
    upper_comeback::State upper_comeback{};
    revival_state::State revival{};
};

void reset(State& state);

} // namespace slotv2::machine_state
