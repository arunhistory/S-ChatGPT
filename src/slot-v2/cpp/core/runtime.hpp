#pragma once
#include <stdint.h>
#include "shared/rng.hpp"
#include "shared/types.hpp"
#include "core/session.hpp"
#include "reel/reel_acquisition.hpp"
#include "core/machine_state.hpp"
#include "core/accounting.hpp"
#include "core/point_ledger.hpp"
#include "special/special_apply.hpp"
#include "core/pending_event.hpp"
#include "at/at_cycle.hpp"
#include "at/at_resolution.hpp"
#include "normal/normal_mode.hpp"
#include "normal/normal_route.hpp"
#include "normal/normal_ceiling.hpp"
#include "normal/normal_ceiling_transition.hpp"
#include "normal/normal_cycle_reset.hpp"
#include "normal/normal_role_trigger.hpp"
#include "normal/normal_flow.hpp"
#include "normal/normal_flow_transition.hpp"
#include "core/setting_profile.hpp"
#include "core/progress_event.hpp"
#include "cz/cz_cycle.hpp"
#include "cz/cz_finalize.hpp"
#include "cz/cz_reward.hpp"
#include "at/at_pending.hpp"
#include "at/at_internal_transition.hpp"
#include "at/at_window_transition.hpp"
#include "section/section_flow.hpp"
#include "section/section_transition.hpp"
#include "at/at_window.hpp"
#include "at/at_stock_restart.hpp"
#include "bonus/bonus_cycle.hpp"
#include "bonus/bonus_transition.hpp"
#include "at/upper_comeback_cycle.hpp"
#include "at/upper_comeback_transition.hpp"
#include "at/at_single_transition.hpp"
#include "at/at_omen.hpp"
#include "normal/normal_at_trigger.hpp"
#include "special/special_zone_pending.hpp"
#include "special/special_zone_transition.hpp"
#include "special/upper_special_transition.hpp"
#include "normal/normal_hit_entry.hpp"
#include "entry/entry_gate_transition.hpp"
#include "revival/revival_cycle.hpp"

namespace slotv2::runtime {

struct State {
    Rng rng{};
    session::State session{};
    acquisition::Result acquisition{};
    machine_state::State machine{};
    accounting::State accounting{};
    point_ledger::State points{};
    bool special_committed{false};
    special_apply::Result last_special_apply{};
    pending_event::State pending{};
    at_cycle::Result at_cycle{};
    at_resolution::Result at_resolution{};
    at_internal_transition::Result at_internal_transition{};
    at_window_transition::Result at_window_transition{};
    normal_mode::Mode normal_mode{normal_mode::Mode::NormalA};
    normal_route::State normal_route{};
    normal_ceiling::Reward normal_ceiling_reward{normal_ceiling::Reward::None};
    normal_ceiling_transition::Result normal_ceiling_transition{};
    bool ceiling_freeze_pending{false};
    normal_role_trigger::DrawResult normal_role_draw{normal_role_trigger::DrawResult::None};
    normal_role_trigger::ApplyResult normal_role_apply{};
    uint8_t setting{6u};
    normal_flow::Result normal_flow_result{};
    normal_flow_transition::Result normal_flow_transition{};
    cz_cycle::Result cz_cycle{};
    cz_finalize::Result cz_finalize{};
    cz_reward::Result cz_reward{};
    bool at_hit_stock_gained{false};
    section_flow::Result last_section_flow{};
    section_transition::Result last_section_transition{};
    at_window::Result at_window{};
    at_stock_restart::Result at_stock_restart{};
    bonus_cycle::Result bonus_cycle{};
    bonus_transition::Result bonus_transition{};
    upper_comeback_cycle::Result upper_comeback_cycle{};
    at_single_transition::Result at_single_transition{};
    at_omen::Game at_omen_game{};
    at_omen::FinalizeOutcome at_omen_finalize{at_omen::FinalizeOutcome::None};
    normal_at_trigger::Result normal_at_trigger{};
    special_zone::HitResult special_zone_result{special_zone::HitResult::None};
    special_zone_transition::Result special_zone_transition{};
    upper_special::Step upper_special_step{};
    normal_hit_entry::Result normal_hit_entry{};
    entry_gate_transition::Result entry_gate_transition{};
    revival_cycle::Game revival_game{};
    revival_cycle::FinalizeResult revival_finalize{};
};

void reset(State& state, uint64_t seed);
uint32_t lever(State& state);
uint32_t stop(State& state, uint32_t reel, uint32_t pressed_position);

// C++内部オーケストレーション用。TSからは呼ばせない。
bool recordCZResult(State& state, bool hit);
void recordNormalHit(State& state, bool was_at);
bool consumeNextHitAT(State& state);

// 本抽選側が「通常当たり」を確定した後だけ呼ぶ。
// BONUS/ATのどちらを選ぶか自体は未確定仕様なので、TSには公開しない。
normal_hit_entry::Result resolveNormalHitAsBonus(State& state);
normal_hit_entry::Result resolveNormalHitAsAT(State& state);

// C++内部会計。TSへ変更権限を出さない。
accounting::Result applyBet(State& state, int medals);
accounting::Result applyPayout(State& state, int medals);
bonus_cycle::Result applyBonusNetGain(State& state, int net_gain);
void startUpperComeback(State& state);

// ATの既存継続・ストック・引戻し処理が全て終わり、完全終了が確定した時だけ呼ぶ。
void startRevivalChallenge(State& state, at_state::Tier ended_tier);

// 復活チャレンジ中に裏の通常当たり枠が成立した時だけ、C++内部から記録する。
void recordRevivalHiddenNormalHit(State& state);

uint32_t phase(const State& state);
uint32_t specialResult(const State& state);
uint32_t completeSpecial(State& state);
uint32_t stoppedPosition(const State& state, uint32_t reel);
uint32_t stopSequence(const State& state, uint32_t order_index);
uint32_t acquisitionPacked(const State& state);
uint32_t lastSpecial(const State& state);
uint32_t lastRole(const State& state);
uint32_t freezeActive(const State& state);

uint32_t machineArea(const State& state);
int64_t sectionDiff(const State& state);
int64_t sectionMinimum(const State& state);
uint64_t sectionCount(const State& state);
uint32_t stockCount(const State& state);
int64_t pointCount(const State& state);
uint32_t atActive(const State& state);
uint32_t atTier(const State& state);
int32_t atGamesLeft(const State& state);
uint32_t specialCommitted(const State& state);
uint32_t bellNavigationPacked(const State& state);
uint32_t bellNavigationNext(const State& state);
uint32_t bellNavigationCorrect(const State& state);
uint32_t pendingEvents(const State& state);
uint32_t atCyclePacked(const State& state);
uint32_t atResolutionPacked(const State& state);
uint32_t normalMode(const State& state);
uint32_t normalRoutePacked(const State& state);
uint32_t normalCeilingTransitionPacked(const State& state);
uint32_t normalActualGames(const State& state);
uint32_t normalDisplayGames(const State& state);
uint32_t czCyclePacked(const State& state);
uint32_t czFinalizePacked(const State& state);
uint32_t sectionRewardPacked(const State& state);
uint32_t sectionTransitionPacked(const State& state);
uint32_t atWindowPacked(const State& state);
uint32_t specialZonePacked(const State& state);
uint32_t bonusPacked(const State& state);
uint32_t bonusCyclePacked(const State& state);
uint32_t bonusTransitionPacked(const State& state);
uint32_t upperComebackPacked(const State& state);
uint32_t atSingleTransitionPacked(const State& state);
uint32_t normalATTriggerPacked(const State& state);
uint32_t specialZoneResultPacked(const State& state);
uint32_t revivalPacked(const State& state);
uint32_t revivalGamePacked(const State& state);
uint32_t revivalFinalizePacked(const State& state);
uint32_t entryGatePacked(const State& state);

} // namespace slotv2::runtime
