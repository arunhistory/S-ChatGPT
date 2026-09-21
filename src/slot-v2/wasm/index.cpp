#include <stdint.h>
#include "../shared/rng.hpp"
#include "../shared/types.hpp"
#include "../lever/index.hpp"
#include "../freeze/index.hpp"
#include "../stop-controller/index.hpp"

namespace {
slotv2::Rng g_rng;
slotv2::LeverResult g_last{};
slotv2::freeze::Directive g_freeze{};
bool g_stopped[3]{false,false,false};
uint8_t g_position[3]{0,0,0};
uint8_t g_stop_count{0};
}

extern "C" {

__attribute__((visibility("default")))
void slot_v2_reset(uint32_t seed_lo, uint32_t seed_hi) {
    const uint64_t seed = (static_cast<uint64_t>(seed_hi) << 32) | seed_lo;
    g_rng.reset(seed);
    g_last = {};
    g_freeze = {};
    g_stop_count = 0;
    for (int i = 0; i < 3; ++i) {
        g_stopped[i] = false;
        g_position[i] = 0;
    }
}

__attribute__((visibility("default")))
uint32_t slot_v2_lever() {
    g_last = slotv2::lever::pull(g_rng);
    g_freeze = slotv2::freeze::begin(g_last.special);
    g_stop_count = 0;
    for (int i = 0; i < 3; ++i) g_stopped[i] = false;

    // low 8bit = RoleFlag / next 8bit = SpecialHit / bit16 = main lottery ran
    return static_cast<uint32_t>(g_last.role)
        | (static_cast<uint32_t>(g_last.special) << 8)
        | (g_last.main_lottery_ran ? (1u << 16) : 0u);
}

__attribute__((visibility("default")))
uint32_t slot_v2_stop(uint32_t reel, uint32_t pressed_position) {
    if (reel > 2u) {
        return (static_cast<uint32_t>(slotv2::stop_shared::ResolveStatus::InvalidReel) << 16);
    }

    if (g_last.special != slotv2::SpecialHit::None) {
        return (static_cast<uint32_t>(slotv2::stop_shared::ResolveStatus::SpecialControlPending) << 16);
    }

    slotv2::stop_shared::Context ctx{};
    ctx.role = g_last.role;
    ctx.reel = static_cast<slotv2::ReelId>(reel);
    ctx.pressed_position = static_cast<uint8_t>(pressed_position % slotv2::kReelSize);
    ctx.stop_order = g_stop_count;

    for (int i = 0; i < 3; ++i) {
        ctx.stopped[i] = g_stopped[i];
        ctx.stopped_position[i] = g_position[i];
    }

    const auto result = slotv2::stop_controller::resolve(ctx);

    if (result.status == slotv2::stop_shared::ResolveStatus::Ok
        || result.status == slotv2::stop_shared::ResolveStatus::RoleMissed) {
        g_stopped[reel] = true;
        g_position[reel] = result.final_position;
        if (g_stop_count < 3) ++g_stop_count;
    }

    // bits 0..7 final position / 8..15 slip / 16..23 status
    return static_cast<uint32_t>(result.final_position)
        | (static_cast<uint32_t>(result.slip) << 8)
        | (static_cast<uint32_t>(result.status) << 16);
}

__attribute__((visibility("default")))
uint32_t slot_v2_stopped_position(uint32_t reel) {
    if (reel > 2u || !g_stopped[reel]) return 0xffffffffu;
    return static_cast<uint32_t>(g_position[reel]);
}

__attribute__((visibility("default")))
uint32_t slot_v2_last_special() {
    return static_cast<uint32_t>(g_last.special);
}

__attribute__((visibility("default")))
uint32_t slot_v2_last_role() {
    return static_cast<uint32_t>(g_last.role);
}

__attribute__((visibility("default")))
uint32_t slot_v2_freeze_active() {
    return g_freeze.active ? 1u : 0u;
}

}
