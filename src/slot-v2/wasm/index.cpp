#include <stdint.h>
#include "../runtime/index.hpp"
#include "../reel-validator/index.hpp"
#include "../reel-read/index.hpp"
#include "../preflight/index.hpp"
#include "../ceiling-catalog/index.hpp"
#include "../normal-progress-view/index.hpp"

namespace {
slotv2::runtime::State g_runtime{};
}

extern "C" {

__attribute__((visibility("default")))
void slot_v2_reset(uint32_t seed_lo, uint32_t seed_hi) {
    const uint64_t seed = (static_cast<uint64_t>(seed_hi) << 32) | seed_lo;
    slotv2::runtime::reset(g_runtime, seed);
}

__attribute__((visibility("default")))
uint32_t slot_v2_lever() {
    return slotv2::runtime::lever(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_stop(uint32_t reel, uint32_t pressed_position) {
    return slotv2::runtime::stop(g_runtime, reel, pressed_position);
}

__attribute__((visibility("default")))
uint32_t slot_v2_phase() {
    return slotv2::runtime::phase(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_special_result() {
    return slotv2::runtime::specialResult(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_complete_special() {
    return slotv2::runtime::completeSpecial(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_stop_sequence(uint32_t order_index) {
    return slotv2::runtime::stopSequence(g_runtime, order_index);
}

__attribute__((visibility("default")))
uint32_t slot_v2_stopped_position(uint32_t reel) {
    return slotv2::runtime::stoppedPosition(g_runtime, reel);
}

__attribute__((visibility("default")))
uint32_t slot_v2_acquisition() {
    return slotv2::runtime::acquisitionPacked(g_runtime);
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
uint32_t slot_v2_preflight() {
    return slotv2::preflight::check();
}

__attribute__((visibility("default")))
uint32_t slot_v2_validate_reel(uint32_t reel) {
    if (reel > 2u) return 0;
    return slotv2::reel_validator::validateAssist(static_cast<slotv2::ReelId>(reel));
}

__attribute__((visibility("default")))
uint32_t slot_v2_reel_ready_mask() {
    return slotv2::reel_validator::readyMask();
}

__attribute__((visibility("default")))
uint32_t slot_v2_validate_left() {
    return slotv2::reel_validator::validateLeft();
}

__attribute__((visibility("default")))
uint32_t slot_v2_machine_area() {
    return slotv2::runtime::machineArea(g_runtime);
}

__attribute__((visibility("default")))
int64_t slot_v2_section_diff() {
    return slotv2::runtime::sectionDiff(g_runtime);
}

__attribute__((visibility("default")))
int64_t slot_v2_section_minimum() {
    return slotv2::runtime::sectionMinimum(g_runtime);
}

__attribute__((visibility("default")))
uint64_t slot_v2_section_count() {
    return slotv2::runtime::sectionCount(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_stock_count() {
    return slotv2::runtime::stockCount(g_runtime);
}

__attribute__((visibility("default")))
int64_t slot_v2_point_count() {
    return slotv2::runtime::pointCount(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_at_active() {
    return slotv2::runtime::atActive(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_at_tier() {
    return slotv2::runtime::atTier(g_runtime);
}

__attribute__((visibility("default")))
int32_t slot_v2_at_games_left() {
    return slotv2::runtime::atGamesLeft(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_special_committed() {
    return slotv2::runtime::specialCommitted(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_bell_navigation() {
    return slotv2::runtime::bellNavigationPacked(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_bell_navigation_next() {
    return slotv2::runtime::bellNavigationNext(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_bell_navigation_correct() {
    return slotv2::runtime::bellNavigationCorrect(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_at_resolution() {
    return slotv2::runtime::atResolutionPacked(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_normal_mode() {
    return slotv2::runtime::normalMode(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_at_cycle() {
    return slotv2::runtime::atCyclePacked(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_pending_events() {
    return slotv2::runtime::pendingEvents(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_normal_progress() {
    return slotv2::normal_progress_view::pack(
        g_runtime.machine.normal_progress
    );
}

__attribute__((visibility("default")))
uint32_t slot_v2_ceiling_count() {
    return static_cast<uint32_t>(slotv2::ceiling_catalog::kCount);
}

__attribute__((visibility("default")))
uint32_t slot_v2_ceiling_at(uint32_t index) {
    if (index >= slotv2::ceiling_catalog::kCount) return 0xffffffffu;
    return static_cast<uint32_t>(slotv2::ceiling_catalog::kValues[index]);
}

__attribute__((visibility("default")))
uint32_t slot_v2_last_special() {
    return slotv2::runtime::lastSpecial(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_last_role() {
    return slotv2::runtime::lastRole(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_freeze_active() {
    return slotv2::runtime::freezeActive(g_runtime);
}

}
