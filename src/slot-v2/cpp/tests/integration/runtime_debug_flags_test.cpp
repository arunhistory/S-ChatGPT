#include <iostream>
#include "core/runtime.hpp"
#include "debug/debug_flags.hpp"
using namespace slotv2;
namespace {
bool spin(runtime::State& s) {
    const auto lever=runtime::lever(s);
    if ((lever>>24)!=0u || s.session.phase!=session::Phase::Stopping) {
        return false;
    }
    for (uint32_t r=0;r<3u;++r) (void)runtime::stop(s,r,0u);
    return s.session.phase==session::Phase::Complete;
}
}
int main() {
    bool ok=true;
    // Every C++ debug-catalogue flag is typed, enumerated and accepted.
    for (const auto& flag : debug::kCatalogue) {
        ok &= debug::valid(flag.channel,flag.value);
    }
    ok &= debug::kCatalogueCount >= 70u;
    ok &= !debug::valid(debug::Channel::ATEvent, 3u);
    ok &= !debug::valid(debug::Channel::Pending, 0xffffffffu);
    ok &= !debug::valid(debug::Channel::NormalRole, 255u);
    {
        runtime::State s{};
        runtime::reset(s,0x33ULL);
        ok &= runtime::armDebugFlag(s,debug::Channel::Mode,
            static_cast<uint32_t>(normal_mode::Mode::Heaven));
        ok &= s.normal_mode==normal_mode::Mode::Heaven;
        ok &= runtime::armDebugFlag(s,debug::Channel::Role,
            static_cast<uint32_t>(RoleFlag::Bell9));
        const auto lever=runtime::lever(s);
        ok &= (lever & 0xffu)==static_cast<uint32_t>(RoleFlag::Bell9);
        ok &= s.debug_flags.last_applied.channel==debug::Channel::Role;
        for (uint32_t reel=0;reel<3u;++reel)
            (void)runtime::stop(s,reel,0u);
        ok &= !runtime::armDebugFlag(s,debug::Channel::ATEvent,
            static_cast<uint32_t>(at_event::Hit));
        ok &= runtime::armDebugFlag(s,debug::Channel::NormalCeiling,
            static_cast<uint32_t>(normal_ceiling::Reward::Bonus));
        ok &= spin(s);
        ok &= s.normal_ceiling_transition.outcome !=
            normal_ceiling_transition::Outcome::None;
        ok &= s.machine.entry_gate.active
            || normal_latent::active(s.latent);
    }
    {
        runtime::State s{};
        runtime::reset(s,0x55ULL);
        ok &= runtime::armDebugFlag(s,debug::Channel::Pending,
            static_cast<uint32_t>(pending_event::NextHitAT));
        ok &= pending_event::has(s.pending,pending_event::NextHitAT);
        ok &= !runtime::armDebugFlag(s,debug::Channel::Role,
            static_cast<uint32_t>(RoleFlag::EntryAT));
        ok &= runtime::armDebugFlag(s,debug::Channel::NormalFlow,
            static_cast<uint32_t>(normal_flow::Reward::CZ));
        ok &= spin(s);
        ok &= s.machine.area==machine_state::Area::CZ;
        ok &= s.machine.cz.active;
        ok &= runtime::armDebugFlag(s,debug::Channel::CZOutcome,
            static_cast<uint32_t>(debug::CZOutcome::ThirdMiss));
        ok &= spin(s);
        ok &= s.machine.area==machine_state::Area::Normal;
        ok &= s.cz_finalize.outcome==
            cz_finalize::Outcome::ThreeMissHitPending;
    }
    {
        runtime::State s{};
        runtime::reset(s,0x77ULL);
        ok &= runtime::armDebugFlag(s,debug::Channel::Special,
            static_cast<uint32_t>(SpecialHit::Freeze));
        ok &= spin(s);
        ok &= s.machine.area==machine_state::Area::AT;
        ok &= s.machine.at.active;
        ok &= s.machine.at.tier==at_state::Tier::Upper;
        ok &= runtime::armDebugFlag(s,debug::Channel::ATEvent,
            static_cast<uint32_t>(at_event::AddGames));
        const auto result=runtime::lever(s);
        ok &= (result>>24)==0u;
        ok &= s.at_cycle.raw.bits==at_event::AddGames;
        for (uint32_t reel=0;reel<3u;++reel)
            (void)runtime::stop(s,reel,0u);
    }
    std::cout<<"slot_v2_runtime_debug_flags_test "
        <<(ok?"PASS":"FAIL")<<" catalogue="<<debug::kCatalogueCount<<'\n';
    return ok?0:1;
}
