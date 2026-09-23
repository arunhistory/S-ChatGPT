#include <iostream>
#include "core/setting_profile.hpp"
#include "normal/normal_high.hpp"
#include "normal/normal_flow.hpp"

int main() {
    bool ok = true;

    {
        const auto p = slotv2::setting_profile::normalRaw(6u);
        ok = ok && p.at_threshold_27 == 50648u;
        ok = ok && p.bonus_threshold_27 == 78981u;
        ok = ok && p.cz_threshold_27 == 116824u;
    }

    ok = ok && slotv2::normal_high::entryThresholdPerThousand(
        slotv2::RoleFlag::Bell15
    ) == 200u;
    ok = ok && slotv2::normal_high::entryThresholdPerThousand(
        slotv2::RoleFlag::Watermelon
    ) == 150u;
    ok = ok && slotv2::normal_high::entryThresholdPerThousand(
        slotv2::RoleFlag::WeakCherry
    ) == 800u;
    ok = ok && slotv2::normal_high::entryThresholdPerThousand(
        slotv2::RoleFlag::WeakChance
    ) == 300u;
    ok = ok && slotv2::normal_high::entryThresholdPerThousand(
        slotv2::RoleFlag::StrongChance
    ) == 900u;
    ok = ok && slotv2::normal_high::entryThresholdPerThousand(
        slotv2::RoleFlag::Miss
    ) == 5u;

    ok = ok && slotv2::normal_high::hitThresholdPerThousand(
        slotv2::RoleFlag::StrongChance
    ) == 990u;

    // Planning must never mutate the current state before third-stop commit.
    {
        slotv2::normal_high::State current{};
        current.active = true;
        current.games = 4u;

        slotv2::Rng rng(0x48494748ULL);
        const auto before = current;

        (void)slotv2::normal_high::draw(
            rng,
            current,
            slotv2::RoleFlag::Replay,
            100u
        );

        ok = ok && current.active == before.active;
        ok = ok && current.games == before.games;
    }

    // Before 51 actual games, an inactive high state cannot newly enter.
    {
        slotv2::normal_high::State current{};
        slotv2::Rng rng(0x504C414EULL);

        const auto r = slotv2::normal_high::draw(
            rng,
            current,
            slotv2::RoleFlag::StrongChance,
            50u
        );

        ok = ok && !r.entered;
        ok = ok && !r.next_state.active;
    }

    if (!ok) {
        std::cerr << "slot_v2_normal_flow_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_normal_flow_test: OK\n";
    return 0;
}
