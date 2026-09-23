#include "section/section_flow.hpp"

namespace slotv2::section_flow {

Result onSectionApply(
    Rng& rng,
    const section::ApplyResult& section_result,
    stock::State& stock_state,
    at_state::Tier tier
) {
    if (!section_result.cut) return {};

    const uint8_t preference = stock::consumeForSection(stock_state);
    const auto reward = section_reward::draw(
        rng,
        tier,
        preference
    );

    return {
        true,
        preference,
        reward
    };
}

} // namespace slotv2::section_flow
