#include <iostream>
#include "section-reward/index.hpp"

int main() {
    bool ok = true;
    slotv2::Rng rng(0x51524354ULL);

    // 到達性とレベル保持を確認。
    for (uint8_t level = 0; level < 4; ++level) {
        bool seenTierUp = false;
        bool seenUpperSpecial = false;
        bool seenSpecial = false;

        for (int i = 0; i < 200000; ++i) {
            const auto low = slotv2::section_reward::draw(
                rng,
                slotv2::at_state::Tier::Lower,
                level
            );

            ok = ok && low.preference_level == level;
            if (low.kind == slotv2::section_reward::Kind::TierUp) {
                seenTierUp = true;
            }

            const auto upper = slotv2::section_reward::draw(
                rng,
                slotv2::at_state::Tier::Upper,
                level
            );

            ok = ok && upper.preference_level == level;
            if (upper.kind == slotv2::section_reward::Kind::Special) {
                seenSpecial = true;
            }
            if (upper.kind == slotv2::section_reward::Kind::UpperSpecial) {
                seenUpperSpecial = true;
            }
        }

        if (level == 0) {
            ok = ok && seenTierUp;
            ok = ok && !seenSpecial && !seenUpperSpecial;
        } else {
            ok = ok && seenTierUp;
            ok = ok && seenSpecial && seenUpperSpecial;
        }
    }

    if (!ok) {
        std::cerr << "slot_v2_section_reward_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_section_reward_test: OK\n";
    return 0;
}
