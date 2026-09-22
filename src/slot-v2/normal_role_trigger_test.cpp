#include <iostream>
#include "normal-role-trigger/index.hpp"

int main() {
    bool ok = true;

    // StrongChance is exactly 1% on its secondary trigger draw.
    uint32_t strong_chance_hits = 0u;
    for (uint64_t seed = 1u; seed <= 20000u; ++seed) {
        slotv2::Rng rng(seed);
        if (slotv2::normal_role_trigger::draw(
                rng,
                slotv2::RoleFlag::StrongChance
            ) == slotv2::normal_role_trigger::DrawResult::LowerAT) {
            ++strong_chance_hits;
        }
    }
    ok = ok && strong_chance_hits > 100u
        && strong_chance_hits < 300u;

    // StrongCherry always produces BONUS or AT, never None.
    bool saw_bonus = false;
    bool saw_lower = false;
    bool saw_middle = false;
    for (uint64_t seed = 1u; seed <= 5000u; ++seed) {
        slotv2::Rng rng(seed);
        const auto r = slotv2::normal_role_trigger::draw(
            rng,
            slotv2::RoleFlag::StrongCherry
        );
        saw_bonus = saw_bonus || r == slotv2::normal_role_trigger::DrawResult::Bonus;
        saw_lower = saw_lower || r == slotv2::normal_role_trigger::DrawResult::LowerAT;
        saw_middle = saw_middle || r == slotv2::normal_role_trigger::DrawResult::MiddleAT;
        ok = ok && r != slotv2::normal_role_trigger::DrawResult::None;
    }
    ok = ok && saw_bonus && saw_lower && saw_middle;

    // Middle AT is queued, not started immediately.
    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);
        slotv2::pending_event::State pending{};

        const auto r = slotv2::normal_role_trigger::apply(
            machine,
            pending,
            slotv2::normal_mode::Mode::NormalA,
            slotv2::normal_role_trigger::DrawResult::MiddleAT
        );

        ok = ok && r.outcome
            == slotv2::normal_role_trigger::Outcome::MiddleATQueued;
        ok = ok && machine.entry_gate.active;
        ok = ok && machine.entry_gate.kind
            == slotv2::entry_gate::Kind::AT;
        ok = ok && machine.entry_gate.at_tier
            == slotv2::at_state::Tier::Middle;
        ok = ok && !machine.at.active;
    }

    if (!ok) {
        std::cerr << "slot_v2_normal_role_trigger_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_normal_role_trigger_test: OK\n";
    return 0;
}
