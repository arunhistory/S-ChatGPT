#include <iostream>
#include "core/runtime.hpp"

int main() {
    bool ok = true;

    slotv2::runtime::State state{};
    slotv2::runtime::reset(state, 0x2400ULL);

    // 区間内最低差枚 -1000 を作る。
    const auto a = slotv2::runtime::applyBet(state, 1000);
    ok = ok && !a.section.cut;
    ok = ok && state.machine.section.current_diff == -1000;
    ok = ok && state.machine.section.minimum_diff == -1000;

    // ストック5個を区間切断時優遇レベル3として消費する。
    slotv2::stock::add(state.machine.stock, 5u);

    // -1000 -> +1399: 最低点から+2399なのでまだ切れない。
    const auto b = slotv2::runtime::applyPayout(state, 2399);
    ok = ok && !b.section.cut;
    ok = ok && state.machine.section.current_diff == 1399;
    ok = ok && state.machine.stock.count == 5u;

    // +1で最低点から+2400。ここで切断。
    const auto c = slotv2::runtime::applyPayout(state, 1);
    ok = ok && c.section.cut;
    ok = ok && c.section.before_reset_diff == 1400;
    ok = ok && c.section.before_reset_minimum == -1000;
    ok = ok && c.section.gain_from_minimum == 2400;
    ok = ok && state.machine.section.current_diff == 0;
    ok = ok && state.machine.section.minimum_diff == 0;
    ok = ok && state.machine.section.section_count == 1u;

    ok = ok && state.last_section_flow.cut;
    ok = ok && state.last_section_flow.preference_level == 3u;
    ok = ok && state.machine.stock.count == 0u;

    // A TierUp earned while payout occurs outside the AT area (e.g. BONUS)
    // must be consumed on the first lever after returning to AT.
    {
        slotv2::runtime::State deferred{};
        slotv2::runtime::reset(deferred, 0x2401ULL);
        slotv2::at_state::start(
            deferred.machine.at,
            slotv2::at_state::Tier::Lower
        );
        deferred.machine.area = slotv2::machine_state::Area::AT;
        deferred.session.phase = slotv2::session::Phase::Complete;
        slotv2::pending_event::add(
            deferred.pending,
            slotv2::pending_event::SectionTierUp
        );
        const auto lever = slotv2::runtime::lever(deferred);
        ok = ok && ((lever >> 24) & 0xffu) == 0u;
        ok = ok && deferred.machine.at.tier
            == slotv2::at_state::Tier::Middle;
        ok = ok && !slotv2::pending_event::has(
            deferred.pending,
            slotv2::pending_event::SectionTierUp
        );
    }

    // Browser-facing per-spin accounting is fully decided by C++.
    {
        slotv2::runtime::State s{};
        slotv2::runtime::reset(s, 0x2410ULL);

        // Normal one-medal: 3 BET -> 1 payout.
        s.session.phase = slotv2::session::Phase::Stopping;
        ok = ok && slotv2::runtime::beginSpinAccounting(s) == 3u;
        s.session.phase = slotv2::session::Phase::Complete;
        s.acquisition = {
            slotv2::RoleFlag::OneMedal,
            slotv2::acquisition::Status::NoPayoutRole,
            0
        };
        ok = ok && slotv2::runtime::settleSpinAccounting(s) == 1u;
        ok = ok && s.accounting.total_diff == -2;

        // Replay makes the next game free instead of inventing a medal payout.
        s.session.phase = slotv2::session::Phase::Stopping;
        ok = ok && slotv2::runtime::beginSpinAccounting(s) == 3u;
        s.session.phase = slotv2::session::Phase::Complete;
        s.acquisition = {
            slotv2::RoleFlag::Replay,
            slotv2::acquisition::Status::Acquired,
            0
        };
        ok = ok && slotv2::runtime::settleSpinAccounting(s) == 0u;
        s.session.phase = slotv2::session::Phase::Stopping;
        ok = ok && slotv2::runtime::beginSpinAccounting(s) == 0u;

        // Upper AT guarantees net +9 for the game: free replay BET -> payout 9.
        s.spin_accounting.area = slotv2::machine_state::Area::AT;
        s.spin_accounting.tier = slotv2::at_state::Tier::Upper;
        s.session.phase = slotv2::session::Phase::Complete;
        s.acquisition = {
            slotv2::RoleFlag::Miss,
            slotv2::acquisition::Status::NoPayoutRole,
            0
        };
        ok = ok && slotv2::runtime::settleSpinAccounting(s) == 9u;
    }

    {
        slotv2::runtime::State s{};
        slotv2::runtime::reset(s, 0x2411ULL);
        slotv2::bonus_transition::start(
            s.machine,
            slotv2::bonus_state::Kind::Regular,
            slotv2::machine_state::Area::Normal
        );
        s.session.phase = slotv2::session::Phase::Stopping;
        ok = ok && slotv2::runtime::beginSpinAccounting(s) == 3u;
        s.session.phase = slotv2::session::Phase::Complete;
        s.acquisition = {
            slotv2::RoleFlag::Miss,
            slotv2::acquisition::Status::NoPayoutRole,
            0
        };
        ok = ok && slotv2::runtime::settleSpinAccounting(s) == 9u;
        ok = ok && s.machine.bonus.medals_left == 44;
        ok = ok && s.bonus_transition.outcome
            == slotv2::bonus_transition::Outcome::None;
    }

    // Reel-less special direct must never inherit the previous game's
    // acquisition/payout.
    {
        slotv2::runtime::State s{};
        slotv2::runtime::reset(s, 0x2412ULL);
        s.session.phase = slotv2::session::Phase::Complete;
        s.acquisition = {
            slotv2::RoleFlag::Bell15,
            slotv2::acquisition::Status::Acquired,
            15
        };
        ok = ok && slotv2::runtime::armDebugFlag(
            s,
            slotv2::debug::Channel::Special,
            static_cast<uint32_t>(slotv2::SpecialHit::UpperAT)
        );
        const auto packed = slotv2::runtime::lever(s);
        ok = ok && ((packed >> 8) & 0xffu)
            == static_cast<uint32_t>(slotv2::SpecialHit::UpperAT);
        ok = ok && s.acquisition.status
            == slotv2::acquisition::Status::NotReady;
        ok = ok && slotv2::runtime::beginSpinAccounting(s) == 3u;
        (void)slotv2::runtime::completeSpecial(s);
        ok = ok && slotv2::runtime::settleSpinAccounting(s) == 0u;
    }

    if (!ok) {
        std::cerr << "slot_v2_runtime_accounting_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_runtime_accounting_test: OK\n";
    return 0;
}
