#include <stdint.h>
#include "../shared/rng.hpp"
#include "../shared/types.hpp"
#include "../lever/index.hpp"
#include "../freeze/index.hpp"

namespace {
slotv2::Rng g_rng;
slotv2::LeverResult g_last{};
slotv2::freeze::Directive g_freeze{};
}

extern "C" {

__attribute__((visibility("default")))
void slot_v2_reset(uint32_t seed_lo, uint32_t seed_hi) {
    const uint64_t seed = (static_cast<uint64_t>(seed_hi) << 32) | seed_lo;
    g_rng.reset(seed);
    g_last = {};
    g_freeze = {};
}

__attribute__((visibility("default")))
uint32_t slot_v2_lever() {
    g_last = slotv2::lever::pull(g_rng);
    g_freeze = slotv2::freeze::begin(g_last.special);

    // low 8bit = RoleFlag / next 8bit = SpecialHit / bit16 = main lottery ran
    return static_cast<uint32_t>(g_last.role)
        | (static_cast<uint32_t>(g_last.special) << 8)
        | (g_last.main_lottery_ran ? (1u << 16) : 0u);
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
