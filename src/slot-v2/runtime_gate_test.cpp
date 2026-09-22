#include <iostream>
#include "runtime/index.hpp"

namespace {

slotv2::CommandStatus commandStatus(uint32_t packed) {
    return static_cast<slotv2::CommandStatus>(
        (packed >> 24) & 0xffu
    );
}

}

int main() {
    bool ok = true;

    {
        slotv2::runtime::State state{};
        slotv2::runtime::reset(state, 0xAAA1ULL);

        slotv2::at_state::start(
            state.machine.at,
            slotv2::at_state::Tier::Middle
        );
        state.machine.area = slotv2::machine_state::Area::AT;
        state.machine.at.games_left = 0;
        slotv2::pending_event::add(
            state.pending,
            slotv2::pending_event::ATWindowEmpty
        );

        state.session.phase = slotv2::session::Phase::Complete;

        const uint32_t lever = slotv2::runtime::lever(state);
        ok = ok
            && commandStatus(lever)
                == slotv2::CommandStatus::RejectedPhase;
    }

    {
        slotv2::runtime::State state{};
        slotv2::runtime::reset(state, 0xAAA2ULL);

        state.machine.area = slotv2::machine_state::Area::CZ;
        state.machine.cz.active = false;
        slotv2::pending_event::add(
            state.pending,
            slotv2::pending_event::CZHit
        );

        state.session.phase = slotv2::session::Phase::Complete;

        const uint32_t lever = slotv2::runtime::lever(state);
        ok = ok
            && commandStatus(lever)
                == slotv2::CommandStatus::RejectedPhase;
    }

    {
        slotv2::runtime::State state{};
        slotv2::runtime::reset(state, 0xAAA3ULL);

        state.machine.area = slotv2::machine_state::Area::Normal;
        state.session.phase = slotv2::session::Phase::Complete;

        const uint32_t lever = slotv2::runtime::lever(state);
        ok = ok
            && commandStatus(lever)
                == slotv2::CommandStatus::Ok;
    }

    if (!ok) {
        std::cerr << "slot_v2_runtime_gate_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_runtime_gate_test: OK\n";
    return 0;
}
