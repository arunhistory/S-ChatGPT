#pragma once
#include <stdint.h>
#include "../shared/rng.hpp"
#include "../shared/types.hpp"
#include "../session/index.hpp"
#include "../acquisition/index.hpp"
#include "../machine-state/index.hpp"
#include "../accounting/index.hpp"
#include "../point-ledger/index.hpp"
#include "../special-apply/index.hpp"
#include "../pending-event/index.hpp"
#include "../at-cycle/index.hpp"
#include "../at-resolution/index.hpp"
#include "../normal-mode/index.hpp"
#include "../progress-event/index.hpp"
#include "../cz-cycle/index.hpp"
#include "../cz-finalize/index.hpp"
#include "../at-pending/index.hpp"
#include "../section-flow/index.hpp"
#include "../section-transition/index.hpp"
#include "../at-window/index.hpp"
#include "../bonus-cycle/index.hpp"
#include "../bonus-transition/index.hpp"
#include "../upper-comeback-cycle/index.hpp"
#include "../at-single-transition/index.hpp"
#include "../normal-at-trigger/index.hpp"
#include "../special-zone-pending/index.hpp"
#include "../normal-hit-entry/index.hpp"

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
    normal_mode::Mode normal_mode{normal_mode::Mode::NormalA};
    cz_cycle::Result cz_cycle{};
    cz_finalize::Result cz_finalize{};
    bool at_hit_stock_gained{false};
    section_flow::Result last_section_flow{};
    section_transition::Result last_section_transition{};
    at_window::Result at_window{};
    bonus_cycle::Result bonus_cycle{};
    bonus_transition::Result bonus_transition{};
    upper_comeback_cycle::Result upper_comeback_cycle{};
    at_single_transition::Result at_single_transition{};
    normal_at_trigger::Result normal_at_trigger{};
    special_zone::HitResult special_zone_result{special_zone::HitResult::None};
    normal_hit_entry::Result normal_hit_entry{};
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

} // namespace slotv2::runtime
