#include <iostream>
#include "runtime/index.hpp"

int main() {
    bool ok = true;

    // NormalA/B resolved BONUS -> regular 50.
    {
        slotv2::runtime::State state{};
        slotv2::runtime::reset(state, 0x1111ULL);
        state.normal_mode = slotv2::normal_mode::Mode::NormalA;

        const auto r = slotv2::runtime::resolveNormalHitAsBonus(state);

        ok = ok && r.outcome == slotv2::normal_hit_entry::Outcome::Bonus;
        ok = ok && state.machine.area == slotv2::machine_state::Area::Bonus;
        ok = ok && state.machine.bonus.kind == slotv2::bonus_state::Kind::Regular;
        ok = ok && state.machine.bonus.medals_left == 50;
        ok = ok && state.machine.normal_progress.normal_hits_without_at == 1u;
    }

    // Heaven/SuperHeaven resolved BONUS -> Episode 80.
    {
        slotv2::runtime::State state{};
        slotv2::runtime::reset(state, 0x2222ULL);
        state.normal_mode = slotv2::normal_mode::Mode::Heaven;

        const auto r = slotv2::runtime::resolveNormalHitAsBonus(state);

        ok = ok && r.outcome == slotv2::normal_hit_entry::Outcome::Bonus;
        ok = ok && state.machine.bonus.kind == slotv2::bonus_state::Kind::Episode;
        ok = ok && state.machine.bonus.medals_left == 80;
    }

    // Next-hit AT guarantee converts a resolved BONUS hit into lower AT.
    {
        slotv2::runtime::State state{};
        slotv2::runtime::reset(state, 0x3333ULL);
        state.machine.normal_progress.next_hit_at_guaranteed = true;
        slotv2::pending_event::add(
            state.pending,
            slotv2::pending_event::NextHitAT
        );

        const auto r = slotv2::runtime::resolveNormalHitAsBonus(state);

        ok = ok && r.outcome == slotv2::normal_hit_entry::Outcome::LowerAT;
        ok = ok && state.machine.area == slotv2::machine_state::Area::AT;
        ok = ok && state.machine.at.tier == slotv2::at_state::Tier::Lower;
        ok = ok && state.machine.at.games_left == 100;
        ok = ok && !state.machine.normal_progress.next_hit_at_guaranteed;
    }

    // A base AT hit enters lower AT and consumes any already-waiting guarantee.
    {
        slotv2::runtime::State state{};
        slotv2::runtime::reset(state, 0x4444ULL);
        state.machine.normal_progress.next_hit_at_guaranteed = true;
        slotv2::pending_event::add(
            state.pending,
            slotv2::pending_event::NextHitAT
        );

        const auto r = slotv2::runtime::resolveNormalHitAsAT(state);

        ok = ok && r.outcome == slotv2::normal_hit_entry::Outcome::LowerAT;
        ok = ok && state.machine.area == slotv2::machine_state::Area::AT;
        ok = ok && !state.machine.normal_progress.next_hit_at_guaranteed;
        ok = ok && !slotv2::pending_event::has(
            state.pending,
            slotv2::pending_event::NextHitAT
        );
    }

    // Special mode stays unresolved rather than inventing a BONUS transition.
    {
        slotv2::runtime::State state{};
        slotv2::runtime::reset(state, 0x5555ULL);
        state.normal_mode = slotv2::normal_mode::Mode::Special;

        const auto r = slotv2::runtime::resolveNormalHitAsBonus(state);

        ok = ok && r.outcome
            == slotv2::normal_hit_entry::Outcome::UnresolvedSpecialMode;
        ok = ok && state.machine.area == slotv2::machine_state::Area::Normal;
    }

    if (!ok) {
        std::cerr << "slot_v2_runtime_normal_hit_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_runtime_normal_hit_test: OK\n";
    return 0;
}
