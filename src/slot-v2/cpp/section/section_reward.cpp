#include "section/section_reward.hpp"

namespace slotv2::section_reward {
namespace {

bool hitPerThousand(Rng& rng, uint16_t threshold) {
    return rng.uniformBelow(1000u) < threshold;
}

}

Result draw(Rng& rng, at_state::Tier tier, uint8_t preference_level) {
    if (preference_level > 3u) preference_level = 3u;

    if (tier == at_state::Tier::Upper) {
        static constexpr uint16_t kUpperRate[4] = {
            0u,   // 0%
            100u, // 10%
            500u, // 50%
            750u  // 75%
        };

        if (!hitPerThousand(rng, kUpperRate[preference_level])) {
            return {Kind::None, preference_level};
        }

        return {
            rng.oneIn(3u) ? Kind::UpperSpecial : Kind::Special,
            preference_level
        };
    }

    static constexpr uint16_t kTierUpRate[4] = {
        5u,   // 0.5%
        100u, // 10%
        250u, // 25%
        500u  // 50%
    };

    return {
        hitPerThousand(rng, kTierUpRate[preference_level])
            ? Kind::TierUp
            : Kind::None,
        preference_level
    };
}

} // namespace slotv2::section_reward
