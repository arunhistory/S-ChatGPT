#pragma once
#include "../shared/rng.hpp"
#include "../section/index.hpp"
#include "../section-reward/index.hpp"
#include "../stock/index.hpp"
#include "../at-state/index.hpp"

namespace slotv2::section_flow {

struct Result {
    bool cut{false};
    uint8_t preference_level{0};
    section_reward::Result reward{};
};

Result onSectionApply(
    Rng& rng,
    const section::ApplyResult& section_result,
    stock::State& stock_state,
    at_state::Tier tier
);

} // namespace slotv2::section_flow
