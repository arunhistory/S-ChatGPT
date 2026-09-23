#include <iostream>
#include "core/runtime.hpp"

int main() {
    bool ok = true;

    // NormalA/B hit is fixed, then waits for RED7/RED7/BAR.
    {
        slotv2::runtime::State state{};
        slotv2::runtime::reset(state, 0x1111ULL);
        state.normal_mode = slotv2::normal_mode::Mode::NormalA;

        const auto r = slotv2::runtime::resolveNormalHitAsBonus(state);

        ok = ok && r.outcome == slotv2::normal_hit_entry::Outcome::Bonus;
        ok = ok && state.machine.area == slotv2::machine_state::Area::Normal;
        ok = ok && !state.machine.bonus.active;
        ok = ok && state.machine.entry_gate.active;
        ok = ok && state.machine.entry_gate.kind == slotv2::entry_gate::Kind::Bonus;
        ok = ok && state.machine.entry_gate.bonus_kind == slotv2::bonus_state::Kind::Regular;
        ok = ok && state.machine.normal_progress.normal_hits_without_at == 1u;
    }

    // Heaven/SuperHeaven hit waits for episode-bonus alignment.
    {
        slotv2::runtime::State state{};
        slotv2::runtime::reset(state, 0x2222ULL);
        state.normal_mode = slotv2::normal_mode::Mode::Heaven;

        const auto r = slotv2::runtime::resolveNormalHitAsBonus(state);

        ok = ok && r.outcome == slotv2::normal_hit_entry::Outcome::Bonus;
        ok = ok && state.machine.entry_gate.active;
        ok = ok && state.machine.entry_gate.bonus_kind == slotv2::bonus_state::Kind::Episode;
    }

    // Next-hit AT guarantee converts the hit into lower-AT entry wait.
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
        ok = ok && state.machine.area == slotv2::machine_state::Area::Normal;
        ok = ok && !state.machine.at.active;
        ok = ok && state.machine.entry_gate.active;
        ok = ok && state.machine.entry_gate.at_tier == slotv2::at_state::Tier::Lower;
        ok = ok && !state.machine.normal_progress.next_hit_at_guaranteed;
    }

    // Base AT hit also waits for RED777 and consumes a waiting guarantee.
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
        ok = ok && state.machine.area == slotv2::machine_state::Area::Normal;
        ok = ok && state.machine.entry_gate.active;
        ok = ok && !state.machine.normal_progress.next_hit_at_guaranteed;
        ok = ok && !slotv2::pending_event::has(
            state.pending,
            slotv2::pending_event::NextHitAT
        );
    }

    // Special-mode normal hit is a regular BONUS and uses the same entry wait.
    {
        slotv2::runtime::State state{};
        slotv2::runtime::reset(state, 0x5555ULL);
        state.normal_mode = slotv2::normal_mode::Mode::Special;

        const auto r = slotv2::runtime::resolveNormalHitAsBonus(state);

        ok = ok && r.outcome == slotv2::normal_hit_entry::Outcome::Bonus;
        ok = ok && state.machine.area == slotv2::machine_state::Area::Normal;
        ok = ok && state.machine.entry_gate.active;
        ok = ok && state.machine.entry_gate.bonus_kind
            == slotv2::bonus_state::Kind::Regular;
    }

    if (!ok) {
        std::cerr << "slot_v2_runtime_normal_hit_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_runtime_normal_hit_test: OK\n";
    return 0;
}
