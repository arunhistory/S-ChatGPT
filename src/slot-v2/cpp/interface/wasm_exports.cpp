#include <stdint.h>
#include "core/runtime.hpp"
#include "reel/reel_validator.hpp"
#include "reel/reel_read.hpp"
#include "core/preflight.hpp"
#include "normal/ceiling_catalog.hpp"
#include "normal/normal_progress_view.hpp"

namespace {
constinit slotv2::runtime::State g_runtime{};
}

extern "C" {

__attribute__((visibility("default")))
void slot_v2_reset(uint32_t seed_lo, uint32_t seed_hi) {
    const uint64_t seed = (static_cast<uint64_t>(seed_hi) << 32) | seed_lo;
    slotv2::runtime::reset(g_runtime, seed);
}

__attribute__((visibility("default")))
uint32_t slot_v2_reset_setting(
    uint32_t seed_lo,
    uint32_t seed_hi,
    uint32_t setting
) {
    if (setting > 0xffu) {
        return static_cast<uint32_t>(
            slotv2::runtime::SettingResetStatus::InvalidSetting
        );
    }

    const uint64_t seed = (static_cast<uint64_t>(seed_hi) << 32) | seed_lo;
    return static_cast<uint32_t>(
        slotv2::runtime::resetWithSetting(
            g_runtime,
            seed,
            static_cast<uint8_t>(setting)
        )
    );
}

__attribute__((visibility("default")))
uint32_t slot_v2_setting() {
    return slotv2::runtime::currentSetting(g_runtime);
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
uint32_t slot_v2_assist_failure_mask(uint32_t reel, uint32_t role) {
    if (reel > 2u) return 0x001fffffu;
    if (role > static_cast<uint32_t>(slotv2::RoleFlag::EntryBonus)) {
        return 0x001fffffu;
    }

    return slotv2::reel_validator::assistFailureMask(
        static_cast<slotv2::ReelId>(reel),
        static_cast<slotv2::RoleFlag>(role)
    );
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
uint32_t slot_v2_normal_route() {
    return slotv2::runtime::normalRoutePacked(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_normal_ceiling_transition() {
    return slotv2::runtime::normalCeilingTransitionPacked(g_runtime);
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
uint32_t slot_v2_normal_actual_games() {
    return slotv2::runtime::normalActualGames(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_normal_display_games() {
    return slotv2::runtime::normalDisplayGames(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_cz_cycle() {
    return slotv2::runtime::czCyclePacked(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_cz_finalize() {
    return slotv2::runtime::czFinalizePacked(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_section_reward() {
    return slotv2::runtime::sectionRewardPacked(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_section_transition() {
    return slotv2::runtime::sectionTransitionPacked(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_special_zone_result() {
    return slotv2::runtime::specialZoneResultPacked(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_special_zone() {
    return slotv2::runtime::specialZonePacked(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_at_window() {
    return slotv2::runtime::atWindowPacked(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_lower_fall_push() {
    return slotv2::runtime::pushLowerFallChallenge(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_lower_fall_challenge() {
    return slotv2::runtime::lowerFallChallengePacked(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_lower_fall_push_outcome() {
    return slotv2::runtime::lowerFallPushOutcome(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_normal_at_trigger() {
    return slotv2::runtime::normalATTriggerPacked(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_at_single_transition() {
    return slotv2::runtime::atSingleTransitionPacked(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_upper_comeback() {
    return slotv2::runtime::upperComebackPacked(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_bonus_state() {
    return slotv2::runtime::bonusPacked(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_bonus_cycle() {
    return slotv2::runtime::bonusCyclePacked(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_bonus_transition() {
    return slotv2::runtime::bonusTransitionPacked(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_revival_state() {
    return slotv2::runtime::revivalPacked(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_revival_game() {
    return slotv2::runtime::revivalGamePacked(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_revival_finalize() {
    return slotv2::runtime::revivalFinalizePacked(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_entry_gate() {
    return slotv2::runtime::entryGatePacked(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_last_special() {
    return slotv2::runtime::lastSpecial(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_last_role() {
    return slotv2::runtime::lastRole(g_runtime);
}

// Accounting belongs to the native engine. The browser sends the bet and
// the measured, accepted acquisition; it never modifies the game lottery.
__attribute__((visibility("default")))
void slot_v2_bet(uint32_t medals) {
    if (medals <= 15u) (void)slotv2::runtime::applyBet(g_runtime, static_cast<int>(medals));
}
__attribute__((visibility("default")))
void slot_v2_payout(uint32_t medals) {
    if (medals <= 1000u) (void)slotv2::runtime::applyPayout(g_runtime, static_cast<int>(medals));
}
__attribute__((visibility("default")))
void slot_v2_bonus_net_gain(int32_t net) {
    if (net >= -100 && net <= 1000) (void)slotv2::runtime::applyBonusNetGain(g_runtime, net);
}
__attribute__((visibility("default")))
int64_t slot_v2_total_diff() {
    return g_runtime.accounting.total_diff;
}

__attribute__((visibility("default")))
uint32_t slot_v2_at_table() {
    return static_cast<uint32_t>(g_runtime.machine.at.table);
}

__attribute__((visibility("default")))
uint32_t slot_v2_normal_flow() {
    return slotv2::runtime::normalFlowPacked(g_runtime);
}
__attribute__((visibility("default")))
uint32_t slot_v2_at_omen() {
    return slotv2::runtime::atOmenPacked(g_runtime);
}
__attribute__((visibility("default")))
uint32_t slot_v2_special_zone_transition() {
    return slotv2::runtime::specialZoneTransitionPacked(g_runtime);
}
__attribute__((visibility("default")))
uint32_t slot_v2_upper_special() {
    return slotv2::runtime::upperSpecialPacked(g_runtime);
}
__attribute__((visibility("default")))
uint32_t slot_v2_chain_zone() {
    return slotv2::runtime::chainZonePacked(g_runtime);
}
__attribute__((visibility("default")))
uint32_t slot_v2_at_stock_restart() {
    return slotv2::runtime::atStockRestartPacked(g_runtime);
}
__attribute__((visibility("default")))
uint32_t slot_v2_entry_gate_transition() {
    return slotv2::runtime::entryGateTransitionPacked(g_runtime);
}
__attribute__((visibility("default")))
uint32_t slot_v2_at_cold() {
    return slotv2::runtime::atCold(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_normal_latent() {
    return slotv2::runtime::normalLatentPacked(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_latent_completion() {
    return static_cast<uint32_t>(g_runtime.last_latent_completion);
}

__attribute__((visibility("default")))
uint32_t slot_v2_debug_count() {
    return slotv2::debug::kCatalogueCount;
}

__attribute__((visibility("default")))
uint32_t slot_v2_debug_channel(uint32_t index) {
    if (index >= slotv2::debug::kCatalogueCount) return 0xffffffffu;
    return static_cast<uint32_t>(slotv2::debug::kCatalogue[index].channel);
}

__attribute__((visibility("default")))
uint32_t slot_v2_debug_value(uint32_t index) {
    if (index >= slotv2::debug::kCatalogueCount) return 0xffffffffu;
    return slotv2::debug::kCatalogue[index].value;
}

__attribute__((visibility("default")))
const char* slot_v2_debug_name(uint32_t index) {
    if (index >= slotv2::debug::kCatalogueCount) return nullptr;
    return slotv2::debug::kCatalogue[index].name;
}

__attribute__((visibility("default")))
uint32_t slot_v2_debug_arm(uint32_t channel, uint32_t value) {
    if (channel > static_cast<uint32_t>(slotv2::debug::Channel::Presentation)) {
        return 0u;
    }
    return slotv2::runtime::armDebugFlag(
        g_runtime, static_cast<slotv2::debug::Channel>(channel), value
    ) ? 1u : 0u;
}

__attribute__((visibility("default")))
uint32_t slot_v2_freeze_active() {
    return slotv2::runtime::freezeActive(g_runtime);
}

__attribute__((visibility("default")))
uint32_t slot_v2_debug_name_ptr(uint32_t index) {
    if (index >= slotv2::debug::kCatalogueCount) return 0u;
    return static_cast<uint32_t>(
        reinterpret_cast<uintptr_t>(slotv2::debug::kCatalogue[index].name)
    );
}

// Browser test adapter: explicit accounting, not silent/automatic C++ spins.
// The frontend must call these once per completed spin.
__attribute__((visibility("default")))
int64_t slot_v2_test_bet(uint32_t medals) {
    if (medals > 3u) return slotv2::runtime::sectionDiff(g_runtime);
    return slotv2::runtime::applyBet(g_runtime, static_cast<int>(medals)).total_diff;
}

__attribute__((visibility("default")))
int64_t slot_v2_test_payout(uint32_t medals) {
    if (medals > 100u) return slotv2::runtime::sectionDiff(g_runtime);
    return slotv2::runtime::applyPayout(g_runtime, static_cast<int>(medals)).total_diff;
}

// BONUS award targets 50/80 by existing C++ engine. This adapter
// specifies the net-gain amount from the browser's test payout model.
__attribute__((visibility("default")))
uint32_t slot_v2_test_bonus_gain(uint32_t net_medals) {
    if (net_medals > 100u) return 0u;
    return static_cast<uint32_t>(
        slotv2::runtime::applyBonusNetGain(g_runtime, static_cast<int>(net_medals)).outcome
    );
}



}
