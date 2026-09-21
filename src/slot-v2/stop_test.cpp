#include <iostream>
#include "shared/types.hpp"
#include "reel-strip/index.hpp"
#include "reel-validator/index.hpp"
#include "stop-controller/index.hpp"

namespace {

bool visibleHasCherry(uint8_t center) {
    const auto strip = slotv2::reel_strip::get(slotv2::ReelId::Left);
    for (int d = -1; d <= 1; ++d) {
        int p = static_cast<int>(center) + d;
        while (p < 0) p += strip.size;
        p %= strip.size;
        if (strip.data[p] == slotv2::Symbol::Cherry) return true;
    }
    return false;
}

bool checkRole(slotv2::RoleFlag role, bool mustHit, bool forbidCherry) {
    bool ok = true;
    for (uint8_t pressed = 0; pressed < slotv2::kReelSize; ++pressed) {
        slotv2::stop_shared::Context ctx{};
        ctx.role = role;
        ctx.reel = slotv2::ReelId::Left;
        ctx.pressed_position = pressed;
        ctx.stop_order = 0;

        const auto r = slotv2::stop_controller::resolve(ctx);

        if (r.slip > slotv2::kMaxSlip) ok = false;
        if (mustHit && r.status != slotv2::stop_shared::ResolveStatus::Ok) ok = false;
        if (forbidCherry && visibleHasCherry(r.final_position)) ok = false;
    }
    return ok;
}

}

int main() {
    bool ok = true;

    // 左配列ではベル/REPLAYが全押下位置から0〜4コマ以内に届くこと。
    ok = checkRole(slotv2::RoleFlag::Bell9, true, true) && ok;
    ok = checkRole(slotv2::RoleFlag::Replay, true, true) && ok;

    // チェリー非成立ゲームで、左の可視3コマに🍒を出さないこと。
    ok = checkRole(slotv2::RoleFlag::Miss, false, true) && ok;
    ok = checkRole(slotv2::RoleFlag::OneMedal, false, true) && ok;
    ok = checkRole(slotv2::RoleFlag::Watermelon, false, true) && ok;

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

    std::cout << "slot_v2_stop_test: OK\n";
    return 0;
}
