#include <iostream>
#include "special-lottery/index.hpp"
#include "special-result/index.hpp"
#include "session/index.hpp"

int main() {
    bool ok = true;

    using slotv2::SpecialHit;

    ok = ok && slotv2::special_lottery::draw(0u) == SpecialHit::Freeze;
    ok = ok && slotv2::special_lottery::draw(1u) == SpecialHit::UpperAT;
    ok = ok && slotv2::special_lottery::draw(4096u) == SpecialHit::UpperAT;
    ok = ok && slotv2::special_lottery::draw(4097u) == SpecialHit::MiddleATStock;
    ok = ok && slotv2::special_lottery::draw(20480u) == SpecialHit::MiddleATStock;
    ok = ok && slotv2::special_lottery::draw(20481u) == SpecialHit::None;

    {
        const auto r = slotv2::special_result::resolve(SpecialHit::MiddleATStock);
        ok = ok
            && r.target == slotv2::special_result::EntryTarget::MiddleAT
            && r.stock == 1
            && !r.freeze;
    }

    {
        const auto r = slotv2::special_result::resolve(SpecialHit::UpperAT);
        ok = ok
            && r.target == slotv2::special_result::EntryTarget::UpperAT
            && r.stock == 0
            && !r.freeze;
    }

    {
        const auto r = slotv2::special_result::resolve(SpecialHit::Freeze);
        ok = ok
            && r.target == slotv2::special_result::EntryTarget::UpperAT
            && r.stock == 1
            && r.freeze;
    }

    {
        slotv2::session::State s{};
        slotv2::session::reset(s);
        ok = ok && s.phase == slotv2::session::Phase::Idle;
        ok = ok && slotv2::session::canLever(s);

        slotv2::LeverResult lever{};
        lever.special = SpecialHit::None;
        lever.role = slotv2::RoleFlag::Replay;
        lever.main_lottery_ran = true;

        ok = ok && slotv2::session::begin(s, lever, {}, {});
        ok = ok && s.phase == slotv2::session::Phase::Stopping;
        ok = ok && !slotv2::session::canLever(s);

        slotv2::stop_shared::Result stop{};
        stop.status = slotv2::stop_shared::ResolveStatus::Ok;

        for (int reel = 0; reel < 3; ++reel) {
            stop.final_position = static_cast<uint8_t>(reel);
            slotv2::session::acceptStop(
                s,
                static_cast<slotv2::ReelId>(reel),
                stop
            );
        }

        ok = ok && s.phase == slotv2::session::Phase::Complete;
        ok = ok && slotv2::session::canLever(s);
    }

    {
        slotv2::session::State s{};
        slotv2::session::reset(s);

        slotv2::LeverResult lever{};
        lever.special = SpecialHit::Freeze;
        lever.role = slotv2::RoleFlag::None;
        lever.main_lottery_ran = false;

        const auto special = slotv2::special_result::resolve(lever.special);
        const auto freeze = slotv2::freeze::begin(lever.special);

        ok = ok && slotv2::session::begin(s, lever, special, freeze);
        ok = ok && s.phase == slotv2::session::Phase::SpecialPending;
        ok = ok && !slotv2::session::canLever(s);

        slotv2::session::completeSpecial(s);
        ok = ok && s.phase == slotv2::session::Phase::Complete;
        ok = ok && slotv2::session::canLever(s);
    }

    if (!ok) {
        std::cerr << "slot_v2_special_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_special_test: OK\n";
    return 0;
}
