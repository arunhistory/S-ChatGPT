#include <iostream>
#include <array>
#include "shared/types.hpp"
#include "reel/reel_strip.hpp"
#include "reel/reel_validator.hpp"
#include "reel/stop/stop_controller.hpp"
#include "reel/reel_acquisition.hpp"
#include "reel/stop/rules/common_rule.hpp"
#include "reel/stop/rules/one_medal_rule.hpp"

namespace {

bool centerOrLowerHasCherry(uint8_t center) {
    const auto strip = slotv2::reel_strip::get(slotv2::ReelId::Left);
    const uint8_t lower = static_cast<uint8_t>((center + 1u) % strip.size);
    return strip.data[center % strip.size] == slotv2::Symbol::Cherry
        || strip.data[lower] == slotv2::Symbol::Cherry;
}

bool checkRole(slotv2::RoleFlag role, bool forbidCherry) {
    bool ok = true;
    for (uint8_t pressed = 0; pressed < slotv2::kReelSize; ++pressed) {
        slotv2::stop_shared::Context ctx{};
        ctx.role = role;
        ctx.reel = slotv2::ReelId::Left;
        ctx.pressed_position = pressed;
        ctx.stop_order = 0;

        const auto r = slotv2::stop_controller::resolve(ctx);

        if (r.slip > slotv2::kMaxSlip) ok = false;

        const bool physicallyStopped =
            r.status == slotv2::stop_shared::ResolveStatus::Ok
            || r.status == slotv2::stop_shared::ResolveStatus::RoleMissed
            || r.status == slotv2::stop_shared::ResolveStatus::SubstituteStop
            || r.status == slotv2::stop_shared::ResolveStatus::AssistGap;

        if (forbidCherry && physicallyStopped && centerOrLowerHasCherry(r.final_position)) {
            ok = false;
        }
    }
    return ok;
}

}

int main() {
    bool ok = true;

    ok = checkRole(slotv2::RoleFlag::Bell9, true) && ok;
    ok = checkRole(slotv2::RoleFlag::Replay, true) && ok;
    ok = checkRole(slotv2::RoleFlag::Miss, true) && ok;
    ok = checkRole(slotv2::RoleFlag::OneMedal, true) && ok;
    ok = checkRole(slotv2::RoleFlag::Watermelon, true) && ok;
    ok = checkRole(slotv2::RoleFlag::WeakChance, true) && ok;
    ok = checkRole(slotv2::RoleFlag::StrongChance, true) && ok;
    ok = checkRole(slotv2::RoleFlag::PenguinChance, true) && ok;
    ok = checkRole(slotv2::RoleFlag::EntryAT, true) && ok;
    ok = checkRole(slotv2::RoleFlag::EntryBonus, true) && ok;

    const uint32_t validator = slotv2::reel_validator::validateLeft();
    const uint32_t required =
        slotv2::reel_validator::LeftCherryHidePossible |
        slotv2::reel_validator::LeftBellGuaranteed |
        slotv2::reel_validator::LeftReplayGuaranteed |
        slotv2::reel_validator::LeftBarLandmarkPair;

    ok = ok && ((validator & required) == required);

    // Bell payout geometry:
    // 9枚 = middle, 15枚 = ↗ (left lower -> right upper),
    // 3枚 = ↘ / top / bottom. A 1枚役 must not steal these payout lines.
    {
        const auto bell9 = slotv2::acquisition::judge(
            slotv2::RoleFlag::Bell9, 3u, 0u, 1u,
            false, false, false
        );
        ok = ok && bell9.status == slotv2::acquisition::Status::Acquired;
        ok = ok && bell9.medals == 9;
    }
    {
        const auto bell15 = slotv2::acquisition::judge(
            slotv2::RoleFlag::Bell15, 2u, 0u, 2u,
            false, false, false
        );
        ok = ok && bell15.status == slotv2::acquisition::Status::Acquired;
        ok = ok && bell15.medals == 15;
    }
    for (const auto positions : {
        std::array<uint8_t,3>{4u,0u,0u},  // ↘
        std::array<uint8_t,3>{4u,1u,2u},  // top
        std::array<uint8_t,3>{2u,20u,0u}  // bottom
    }) {
        const auto bell3 = slotv2::acquisition::judge(
            slotv2::RoleFlag::ThreeMedal,
            positions[0], positions[1], positions[2],
            false, false, false
        );
        ok = ok && bell3.status == slotv2::acquisition::Status::Acquired;
        ok = ok && bell3.medals == 3;
    }
    {
        slotv2::stop_shared::Context ctx{};
        ctx.role = slotv2::RoleFlag::OneMedal;
        ctx.reel = slotv2::ReelId::Right;
        ctx.stop_order = 2;
        ctx.stopped[0] = true;
        ctx.stopped[1] = true;
        ctx.stopped_position[0] = 4u;
        ctx.stopped_position[1] = 0u;
        const auto right = slotv2::reel_strip::get(slotv2::ReelId::Right);
        ok = ok && slotv2::stop_rules::completesVisibleBellLine(ctx,right,0u);
        ok = ok && !slotv2::stop_rules::one_medal::accepts(ctx,right,0u);
    }

    // Approved chance substitutes are zero-payout chance results, never a one-medal role.
    {
        const auto weak = slotv2::acquisition::judge(
            slotv2::RoleFlag::WeakChance,
            3u, 0u, 3u, // BELL / BELL / SNOW
            false, false, false
        );
        ok = ok && weak.internal_role == slotv2::RoleFlag::WeakChance;
        ok = ok && weak.medals == 0;
        ok = ok && weak.status == slotv2::acquisition::Status::NoPayoutRole;
    }

    {
        const auto strong = slotv2::acquisition::judge(
            slotv2::RoleFlag::StrongChance,
            12u, 3u, 3u, // WATERMELON / WATERMELON / SNOW
            false, false, false
        );
        ok = ok && strong.internal_role == slotv2::RoleFlag::StrongChance;
        ok = ok && strong.medals == 0;
        ok = ok && strong.status == slotv2::acquisition::Status::NoPayoutRole;
    }

    if (!ok) {
        std::cerr << "slot_v2_stop_test: FAILED validator=" << validator << "\n";
        return 1;
    }

    std::cout << "slot_v2_stop_test: OK validator=" << validator << "\n";
    return 0;
}
