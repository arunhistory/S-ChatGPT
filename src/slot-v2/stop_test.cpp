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
            || r.status == slotv2::stop_shared::ResolveStatus::SubstituteStop;

        if (forbidCherry && physicallyStopped && visibleHasCherry(r.final_position)) {
            ok = false;
        }
    }
    return ok;
}

}

int main() {
    bool ok = true;

    // すべての停止結果は0〜4コマ。非チェリー役では可視🍒禁止を守る。
    ok = checkRole(slotv2::RoleFlag::Bell9, true) && ok;
    ok = checkRole(slotv2::RoleFlag::Replay, true) && ok;
    ok = checkRole(slotv2::RoleFlag::Miss, true) && ok;
    ok = checkRole(slotv2::RoleFlag::OneMedal, true) && ok;
    ok = checkRole(slotv2::RoleFlag::Watermelon, true) && ok;
    ok = checkRole(slotv2::RoleFlag::WeakChance, true) && ok;
    ok = checkRole(slotv2::RoleFlag::StrongChance, true) && ok;
    ok = checkRole(slotv2::RoleFlag::PenguinChance, true) && ok;

    // 確定済み左配列では「🍉/BAR/🍒目印が2組」と「🍒を蹴れること」を必須条件にする。
    // 🔔/REPLAY全位置保証は validator の診断bitとして残し、未解決条件を隠さない。
    const uint32_t validator = slotv2::reel_validator::validateLeft();
    const uint32_t required =
        slotv2::reel_validator::LeftCherryHidePossible |
        slotv2::reel_validator::LeftBarLandmarkPair;

    ok = ok && ((validator & required) == required);

    if (!ok) {
        std::cerr << "slot_v2_stop_test: FAILED validator=" << validator << "\n";
        return 1;
    }

    std::cout << "slot_v2_stop_test: OK validator=" << validator << "\n";
    return 0;
}
