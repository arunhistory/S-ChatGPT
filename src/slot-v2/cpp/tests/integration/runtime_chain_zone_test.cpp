#include <iostream>
#include "core/runtime.hpp"

int main() {
    bool ok=true;
    bool finished=false;
    for(uint64_t seed=1;seed<200 && !finished;++seed) {
        slotv2::runtime::State state{};
        slotv2::runtime::reset(state,seed);
        slotv2::at_state::start(
            state.machine.at, slotv2::at_state::Tier::Lower
        );
        state.machine.area=slotv2::machine_state::Area::AT;
        slotv2::chain_zone::start(state.machine.chain_zone);
        state.machine.chain_zone.games_left=1u;
        state.machine.chain_zone.continuations=8u;
        const auto lever=slotv2::runtime::lever(state);
        ok=ok && (lever>>8 & 0xffu)==0u;
        if (!state.chain_zone_step.ended) continue;
        finished=true;
        ok=ok && state.machine.at.games_left==100;
        ok=ok && !state.at_cycle.active;
        (void)slotv2::runtime::stop(state,0u,0u);
        (void)slotv2::runtime::stop(state,1u,0u);
        (void)slotv2::runtime::stop(state,2u,0u);
        ok=ok && state.machine.chain_zone.bonus_remaining==3u;
        ok=ok && state.machine.entry_gate.active;
        ok=ok && state.machine.entry_gate.bonus_return_to_at;
        ok=ok && state.machine.entry_gate.chain_multi_bonus;
        ok=ok && state.machine.entry_gate.kind
            ==slotv2::entry_gate::Kind::Bonus;
    }
    ok=ok && finished;
    if (!ok) {
        std::cerr << "slot_v2_runtime_chain_zone_test: FAILED\n";
        return 1;
    }
    std::cout << "slot_v2_runtime_chain_zone_test: OK\n";
    return 0;
}
