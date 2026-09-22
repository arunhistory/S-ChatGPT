#include <iostream>
#include "shared/types.hpp"
#include "reel-strip/index.hpp"
#include "reel-validator/index.hpp"
#include "stop-controller/index.hpp"

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

    const uint32_t validator = slotv2::reel_validator::validateLeft();
    const uint32_t required =
        slotv2::reel_validator::LeftCherryHidePossible |
        slotv2::reel_validator::LeftBellGuaranteed |
        slotv2::reel_validator::LeftReplayGuaranteed |
        slotv2::reel_validator::LeftBarLandmarkPair;

    ok = ok && ((validator & required) == required);

    if (!ok) {
        std::cerr << "slot_v2_stop_test: FAILED validator=" << validator << "\n";
        return 1;
    }

    std::cout << "slot_v2_stop_test: OK validator=" << validator << "\n";
    return 0;
}
