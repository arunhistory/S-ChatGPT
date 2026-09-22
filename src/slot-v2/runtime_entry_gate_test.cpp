#include <iostream>
#include "runtime/index.hpp"

namespace {

bool stopExact(
    slotv2::runtime::State& state,
    uint8_t left,
    uint8_t middle,
    uint8_t right
) {
    const auto a = slotv2::runtime::stop(state, 0u, left);
    const auto b = slotv2::runtime::stop(state, 1u, middle);
    const auto c = slotv2::runtime::stop(state, 2u, right);

    return ((a >> 16) & 0xffu) <= static_cast<uint32_t>(slotv2::stop_shared::ResolveStatus::AssistGap)
        && ((b >> 16) & 0xffu) <= static_cast<uint32_t>(slotv2::stop_shared::ResolveStatus::AssistGap)
        && ((c >> 16) & 0xffu) <= static_cast<uint32_t>(slotv2::stop_shared::ResolveStatus::AssistGap);
}

}

int main() {
    bool ok = true;

    {
        slotv2::runtime::State state{};
        slotv2::runtime::reset(state, 0xE771ULL);

        const auto queued = slotv2::runtime::resolveNormalHitAsAT(state);
        ok = ok && queued.outcome == slotv2::normal_hit_entry::Outcome::LowerAT;
        ok = ok && state.machine.entry_gate.active;
        ok = ok && !state.machine.at.active;

        bool started = false;
        for (int attempt = 0; attempt < 64 && !started; ++attempt) {
            const uint32_t lever = slotv2::runtime::lever(state);
            const auto role = static_cast<slotv2::RoleFlag>(lever & 0xffu);

            if (role == slotv2::RoleFlag::EntryAT) {
                // RED7 indices: left=4, middle=8, right=11 (zero-based).
                ok = ok && stopExact(state, 4u, 8u, 11u);
                started = state.machine.at.active;
            } else {
                ok = ok && role == slotv2::RoleFlag::Miss;
                ok = ok && stopExact(state, 0u, 0u, 0u);
                ok = ok && state.machine.entry_gate.active;
                ok = ok && !state.machine.at.active;
            }
        }

        ok = ok && started;
        ok = ok && state.machine.area == slotv2::machine_state::Area::AT;
        ok = ok && state.machine.at.tier == slotv2::at_state::Tier::Lower;
        ok = ok && state.machine.at.games_left == 100;
        ok = ok && !state.machine.entry_gate.active;
    }

    {
        slotv2::runtime::State state{};
        slotv2::runtime::reset(state, 0xB771ULL);
        state.normal_mode = slotv2::normal_mode::Mode::NormalA;

        const auto queued = slotv2::runtime::resolveNormalHitAsBonus(state);
        ok = ok && queued.outcome == slotv2::normal_hit_entry::Outcome::Bonus;
        ok = ok && state.machine.entry_gate.active;
        ok = ok && !state.machine.bonus.active;

        bool started = false;
        for (int attempt = 0; attempt < 64 && !started; ++attempt) {
            const uint32_t lever = slotv2::runtime::lever(state);
            const auto role = static_cast<slotv2::RoleFlag>(lever & 0xffu);

            if (role == slotv2::RoleFlag::EntryBonus) {
                // RED7/RED7/BAR indices: left=4, middle=8, right=12.
                ok = ok && stopExact(state, 4u, 8u, 12u);
                started = state.machine.bonus.active;
            } else {
                ok = ok && role == slotv2::RoleFlag::Miss;
                ok = ok && stopExact(state, 0u, 0u, 0u);
                ok = ok && state.machine.entry_gate.active;
                ok = ok && !state.machine.bonus.active;
            }
        }

        ok = ok && started;
        ok = ok && state.machine.area == slotv2::machine_state::Area::Bonus;
        ok = ok && state.machine.bonus.kind == slotv2::bonus_state::Kind::Regular;
        ok = ok && state.machine.bonus.medals_left == 50;
        ok = ok && !state.machine.entry_gate.active;
    }


    // Entry wait must outrank an already-pending AT-window end.
    {
        slotv2::runtime::State state{};
        slotv2::runtime::reset(state, 0xA771ULL);

        slotv2::at_state::start(
            state.machine.at,
            slotv2::at_state::Tier::Lower
        );
        state.machine.area = slotv2::machine_state::Area::AT;
        state.machine.at.games_left = 0;
        slotv2::entry_gate::queueBonus(
            state.machine.entry_gate,
            slotv2::bonus_state::Kind::Regular,
            true
        );
        slotv2::pending_event::add(
            state.pending,
            slotv2::pending_event::ATWindowEmpty
        );

        bool spun = false;
        for (int attempt = 0; attempt < 64 && !spun; ++attempt) {
            const uint32_t lever = slotv2::runtime::lever(state);
            const auto status = static_cast<slotv2::CommandStatus>((lever >> 24) & 0xffu);
            const auto role = static_cast<slotv2::RoleFlag>(lever & 0xffu);

            ok = ok && status == slotv2::CommandStatus::Ok;
            ok = ok && state.machine.area == slotv2::machine_state::Area::AT;
            ok = ok && state.machine.at.active;
            ok = ok && state.machine.at.games_left == 0;

            if (role == slotv2::RoleFlag::EntryBonus) {
                ok = ok && stopExact(state, 4u, 8u, 12u);
                spun = true;
            } else {
                ok = ok && role == slotv2::RoleFlag::Miss;
                ok = ok && stopExact(state, 0u, 0u, 0u);
            }
        }

        ok = ok && spun;
        ok = ok && state.machine.area == slotv2::machine_state::Area::Bonus;
        ok = ok && state.machine.bonus.active;
        ok = ok && slotv2::pending_event::has(
            state.pending,
            slotv2::pending_event::ATWindowEmpty
        );
    }

    if (!ok) {
        std::cerr << "slot_v2_runtime_entry_gate_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_runtime_entry_gate_test: OK\n";
    return 0;
}
