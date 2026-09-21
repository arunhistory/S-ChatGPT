#include <stdint.h>
#include "../shared/rng.hpp"
#include "../shared/types.hpp"
#include "../lever/index.hpp"
#include "../freeze/index.hpp"
#include "../stop-controller/index.hpp"
#include "../reel-validator/index.hpp"
#include "../reel-read/index.hpp"
#include "../session/index.hpp"
#include "../acquisition/index.hpp"

namespace {
slotv2::Rng g_rng;
slotv2::session::State g_session{};
slotv2::acquisition::Result g_acquisition{};
}

extern "C" {

__attribute__((visibility("default")))
void slot_v2_reset(uint32_t seed_lo, uint32_t seed_hi) {
    const uint64_t seed = (static_cast<uint64_t>(seed_hi) << 32) | seed_lo;
    g_rng.reset(seed);
    slotv2::session::reset(g_session);
    g_acquisition = {};
}

__attribute__((visibility("default")))
uint32_t slot_v2_lever() {
    const auto lever = slotv2::lever::pull(g_rng);
    const auto freeze = slotv2::freeze::begin(lever.special);
    slotv2::session::begin(g_session, lever, freeze);
    g_acquisition = {};

    // low 8bit = RoleFlag / next 8bit = SpecialHit / bit16 = main lottery ran
    return static_cast<uint32_t>(lever.role)
        | (static_cast<uint32_t>(lever.special) << 8)
        | (lever.main_lottery_ran ? (1u << 16) : 0u);
}

__attribute__((visibility("default")))
uint32_t slot_v2_stop(uint32_t reel, uint32_t pressed_position) {
    if (reel > 2u) {
        return (static_cast<uint32_t>(slotv2::stop_shared::ResolveStatus::InvalidReel) << 16);
    }

    const auto reel_id = static_cast<slotv2::ReelId>(reel);

    // 同じリールを二度止めて停止順を壊さない。
    if (g_session.stopped[reel]) {
        return (static_cast<uint32_t>(slotv2::stop_shared::ResolveStatus::InvalidReel) << 16)
            | static_cast<uint32_t>(g_session.position[reel]);
    }

    if (g_session.lever.special != slotv2::SpecialHit::None) {
        return (static_cast<uint32_t>(slotv2::stop_shared::ResolveStatus::SpecialControlPending) << 16);
    }

    const auto ctx = slotv2::session::makeStopContext(
        g_session,
        reel_id,
        static_cast<uint8_t>(pressed_position)
    );

    const auto result = slotv2::stop_controller::resolve(ctx);
    slotv2::session::acceptStop(g_session, reel_id, result);

    if (g_session.stop_count == 3u) {
        g_acquisition = slotv2::acquisition::judge(
            g_session.lever.role,
            g_session.position[0],
            g_session.position[1],
            g_session.position[2]
        );
    }

    // bits 0..7 final position / 8..15 slip / 16..23 status
    return static_cast<uint32_t>(result.final_position)
        | (static_cast<uint32_t>(result.slip) << 8)
        | (static_cast<uint32_t>(result.status) << 16);
}

__attribute__((visibility("default")))
uint32_t slot_v2_stopped_position(uint32_t reel) {
    if (reel > 2u || !g_session.stopped[reel]) return 0xffffffffu;
    return static_cast<uint32_t>(g_session.position[reel]);
}

__attribute__((visibility("default")))
uint32_t slot_v2_acquisition() {
    // low 8bit = acquisition status / next 8bit = internal RoleFlag / high 16bit = medals
    return static_cast<uint32_t>(g_acquisition.status)
        | (static_cast<uint32_t>(g_acquisition.internal_role) << 8)
        | ((static_cast<uint32_t>(g_acquisition.medals) & 0xffffu) << 16);
}

__attribute__((visibility("default")))
uint32_t slot_v2_symbol_at(uint32_t reel, uint32_t position) {
    if (reel > 2u) return static_cast<uint32_t>(slotv2::Symbol::Unknown);
    return static_cast<uint32_t>(
        slotv2::reel_read::at(
            static_cast<slotv2::ReelId>(reel),
            static_cast<uint8_t>(position)
        )
    );
}

__attribute__((visibility("default")))
uint32_t slot_v2_visible_symbol(uint32_t reel, uint32_t center_position, int32_t row_offset) {
    if (reel > 2u || row_offset < -1 || row_offset > 1) {
        return static_cast<uint32_t>(slotv2::Symbol::Unknown);
    }
    return static_cast<uint32_t>(
        slotv2::reel_read::visible(
            static_cast<slotv2::ReelId>(reel),
            static_cast<uint8_t>(center_position),
            static_cast<int8_t>(row_offset)
        )
    );
}

__attribute__((visibility("default")))
uint32_t slot_v2_validate_left() {
    return slotv2::reel_validator::validateLeft();
}

__attribute__((visibility("default")))
uint32_t slot_v2_last_special() {
    return static_cast<uint32_t>(g_session.lever.special);
}

__attribute__((visibility("default")))
uint32_t slot_v2_last_role() {
    return static_cast<uint32_t>(g_session.lever.role);
}

__attribute__((visibility("default")))
uint32_t slot_v2_freeze_active() {
    return g_session.freeze.active ? 1u : 0u;
}

}
