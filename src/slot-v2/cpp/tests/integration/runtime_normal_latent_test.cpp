#include <cmath>
#include <iostream>
#include "core/runtime.hpp"
#include "normal/normal_latent.hpp"

using namespace slotv2;
namespace {
bool fullSpin(runtime::State& s) {
    const auto lever = runtime::lever(s);
    if ((lever >> 24) != 0u || s.session.phase != session::Phase::Stopping) {
        return false;
    }
    for (uint32_t reel = 0u; reel < 3u; ++reel) {
        (void)runtime::stop(s, reel, 0u);
    }
    return s.session.phase == session::Phase::Complete;
}
bool testOmenAndCZ() {
    bool ok = true;
    {
        runtime::State s{};
        runtime::reset(s, 0x123412341234ULL);
        ok &= runtime::armDebugFlag(s, debug::Channel::Presentation,
            static_cast<uint32_t>(normal_latent::Route::Omen));
        ok &= runtime::armDebugFlag(s, debug::Channel::NormalFlow,
            static_cast<uint32_t>(normal_flow::Reward::Bonus));
        ok &= fullSpin(s);
        const int expected = s.latent.omen_games_total;
        ok &= expected >= 10 && expected <= 25;
        ok &= s.latent.stage == normal_latent::Stage::Omen;
        ok &= s.machine.area == machine_state::Area::Normal;
        ok &= !s.machine.entry_gate.active;
        ok &= s.latent.awarded_gate.kind == entry_gate::Kind::Bonus;
        for (int i = 0; i < expected; ++i) {
            ok &= fullSpin(s);
            if (i + 1 < expected) {
                ok &= s.latent.stage == normal_latent::Stage::Omen;
                ok &= !s.machine.entry_gate.active;
            }
        }
        ok &= s.latent.stage == normal_latent::Stage::None;
        ok &= s.machine.entry_gate.active;
        ok &= s.machine.entry_gate.kind == entry_gate::Kind::Bonus;
        ok &= s.last_latent_completion == normal_latent::Completion::GateReady;
        ok &= s.machine.normal.actual_games >= static_cast<uint32_t>(expected);
    }
    {
        runtime::State s{};
        runtime::reset(s, 0x887766ULL);
        ok &= runtime::armDebugFlag(s, debug::Channel::Presentation,
            static_cast<uint32_t>(normal_latent::Route::OmenCZ));
        ok &= runtime::armDebugFlag(s, debug::Channel::NormalRole,
            static_cast<uint32_t>(normal_role_trigger::DrawResult::MiddleAT));
        ok &= fullSpin(s);
        ok &= s.latent.stage == normal_latent::Stage::Omen;
        ok &= s.latent.awarded_gate.at_tier == at_state::Tier::Middle;
        const auto waiting = s.latent.omen_games_left;
        for (unsigned i = 0; i < waiting; ++i) ok &= fullSpin(s);
        ok &= s.latent.stage == normal_latent::Stage::ScriptedCZ;
        ok &= s.machine.area == machine_state::Area::CZ;
        ok &= s.machine.cz.active;
        for (int i = 0; i < 10 && s.machine.cz.active; ++i) {
            ok &= fullSpin(s);
        }
        ok &= s.latent.stage == normal_latent::Stage::None;
        ok &= s.machine.area == machine_state::Area::Normal;
        ok &= s.machine.entry_gate.active;
        ok &= s.machine.entry_gate.kind == entry_gate::Kind::AT;
        // The scripted CZ cannot downgrade or reroll the internally
        // guaranteed Middle AT.
        ok &= s.machine.entry_gate.at_tier == at_state::Tier::Middle;
    }
    {
        runtime::State s{};
        runtime::reset(s, 0x334455ULL);
        ok &= runtime::armDebugFlag(s, debug::Channel::Presentation,
            static_cast<uint32_t>(normal_latent::Route::NormalEvent));
        ok &= runtime::armDebugFlag(s, debug::Channel::NormalFlow,
            static_cast<uint32_t>(normal_flow::Reward::LowerAT));
        ok &= fullSpin(s);
        ok &= s.latent.stage == normal_latent::Stage::Omen;
        ok &= !s.machine.entry_gate.active;
        // The 1% event route has a HARD minimum of ten completed games.
        for (int i = 0; i < 9; ++i) {
            ok &= fullSpin(s);
            ok &= normal_latent::active(s.latent);
            ok &= !s.machine.entry_gate.active;
        }
        ok &= runtime::armDebugFlag(s, debug::Channel::Role,
            static_cast<uint32_t>(RoleFlag::WeakChance));
        ok &= fullSpin(s);
        ok &= !normal_latent::active(s.latent);
        ok &= s.machine.entry_gate.active;
        ok &= s.machine.entry_gate.kind == entry_gate::Kind::AT;
        ok &= s.last_latent_completion
            == normal_latent::Completion::NormalEventReady;
    }
    {
        // The 1% event route also has a bounded, physical-role-neutral
        // fallback at its preselected 10-25 G deadline.
        Rng rng(0x55667788ULL);
        normal_latent::State latent{};
        entry_gate::State gate{};
        entry_gate::queueAT(gate, at_state::Tier::Middle);
        const auto result = normal_latent::capture(
            rng, latent, gate, true,
            static_cast<uint8_t>(normal_latent::Route::NormalEvent)
        );
        ok &= result.captured;
        ok &= result.omen_games >= 10u && result.omen_games <= 25u;
        for (int i = 0; i < result.omen_games; ++i) {
            const auto completion = normal_latent::completeOmenGame(
                latent, gate, RoleFlag::OneMedal
            );
            ok &= completion == (i + 1 == result.omen_games
                ? normal_latent::Completion::NormalEventReady
                : normal_latent::Completion::None);
            if (i + 1 < result.omen_games) ok &= !gate.active;
        }
        ok &= gate.active && gate.at_tier == at_state::Tier::Middle;
    }
    {
        // A successful ordinary CZ award is delayed, but never loops
        // through a second scripted CZ.
        runtime::State s{};
        runtime::reset(s, 0x22334455ULL);
        s.machine.area = machine_state::Area::CZ;
        cz_state::start(s.machine.cz);
        ok &= runtime::armDebugFlag(s, debug::Channel::CZOutcome,
            static_cast<uint32_t>(debug::CZOutcome::Hit));
        ok &= fullSpin(s);
        ok &= s.machine.area == machine_state::Area::Normal;
        ok &= (s.machine.entry_gate.active
            || s.latent.stage == normal_latent::Stage::Omen);
        ok &= s.latent.route != normal_latent::Route::OmenCZ;
    }
    return ok;
}
bool testRouteDistribution() {
    Rng rng(0xA5A5A5A55A5A5A5AULL);
    int normal_event = 0, omen = 0, scripted = 0;
    for (int i = 0; i < 100000; ++i) {
        normal_latent::State latent{};
        entry_gate::State gate{};
        entry_gate::queueBonus(gate, bonus_state::Kind::Regular, false);
        const auto result = normal_latent::capture(rng, latent, gate);
        if (result.route == normal_latent::Route::NormalEvent)
            ++normal_event;
        if (result.route == normal_latent::Route::Omen) ++omen;
        if (result.route == normal_latent::Route::OmenCZ) ++scripted;
        if (!result.captured) return false;
        if (result.omen_games < 10u || result.omen_games > 25u
            || gate.active || !normal_latent::active(latent)) {
            return false;
        }
    }
    std::cout << "presentation: event=" << normal_event
        << " omen=" << omen << " omen+CZ=" << scripted << '\n';
    return std::abs(normal_event - 1000) < 220
        && std::abs(omen - 4000) < 450
        && std::abs(scripted - 95000) < 550
        && omen + normal_event + scripted == 100000;
}
}
int main() {
    const bool valid = testOmenAndCZ() && testRouteDistribution();
    std::cout << "slot_v2_runtime_normal_latent_test "
        << (valid ? "PASS" : "FAIL") << '\n';
    return valid ? 0 : 1;
}
