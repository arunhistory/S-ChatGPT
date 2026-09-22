#include "index.hpp"

namespace slotv2::normal_flow {

Result draw(
    Rng& rng,
    const normal_high::State& high,
    RoleFlag role,
    uint32_t actual_games,
    uint8_t setting
) {
    Result out{};

    out.high = normal_high::draw(
        rng,
        high,
        role,
        actual_games
    );

    if (out.high.reward == normal_high::Reward::CZ) {
        out.reward = Reward::CZ;
        return out;
    }

    if (out.high.reward == normal_high::Reward::Bonus) {
        out.reward = Reward::Bonus;
        return out;
    }

    switch (normal_raw_lottery::draw(rng, setting)) {
        case normal_raw_lottery::Reward::LowerAT:
            out.reward = Reward::LowerAT;
            break;
        case normal_raw_lottery::Reward::Bonus:
            out.reward = Reward::Bonus;
            break;
        case normal_raw_lottery::Reward::CZ:
            out.reward = Reward::CZ;
            break;
        case normal_raw_lottery::Reward::None:
        default:
            break;
    }

    return out;
}

} // namespace slotv2::normal_flow
