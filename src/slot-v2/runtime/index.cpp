#include "index.hpp"
#include "../lever/index.hpp"
#include "../freeze/index.hpp"
#include "../special-result/index.hpp"
#include "../stop-controller/index.hpp"
#include "../game-finalize/index.hpp"
#include "../stock-lottery/index.hpp"
#include "../at-pending/index.hpp"
#include "../section-flow/index.hpp"
#include "../cz-finalize/index.hpp"
#include "../cz-reward/index.hpp"
#include "../section-transition/index.hpp"
#include "../at-window/index.hpp"
#include "../bonus-cycle/index.hpp"
#include "../bonus-transition/index.hpp"
#include "../upper-comeback-cycle/index.hpp"
#include "../at-single-transition/index.hpp"
#include "../normal-at-trigger/index.hpp"
#include "../special-zone-pending/index.hpp"
#include "../normal-hit-entry/index.hpp"
#include "../revival-cycle/index.hpp"

namespace slotv2::runtime {
namespace {

void publishSectionReward(
    const section_flow::Result& flow,
    pending_event::State& pending
) {
    if (!flow.cut) return;

    switch (flow.reward.kind) {
        case section_reward::Kind::TierUp:
            pending_event::add(pending, pending_event::SectionTierUp);
            break;
        case section_reward::Kind::Special:
            pending_event::add(pending, pending_event::SectionSpecial);
            break;
        case section_reward::Kind::UpperSpecial:
            pending_event::add(pending, pending_event::SectionUpperSpec);
            break;
        case section_reward::Kind::None:
        default:
            break;
    }
}

void handleSection(
    State& state,
    const section::ApplyResult& section_result
) {
    state.last_section_flow = section_flow::onSectionApply(
        state.rng,
        section_result,
        state.machine.stock,
        state.machine.at.tier
    );

    publishSectionReward(
        state.last_section_flow,
        state.pending
    );

    state.last_section_transition = section_transition::apply(
        state.machine,
        state.pending,
        state.last_section_flow
    );
}

} // namespace

void reset(State& state, uint64_t seed) {
    state.rng.reset(seed);
    session::reset(state.session);
    state.acquisition = {};
    machine_state::reset(state.machine);
    state.accounting = {};
    state.points = {};
    state.special_committed = false;
    state.last_special_apply = {};
    pending_event::clear(state.pending);
    state.at_cycle = {};
    state.at_resolution = {};
    state.normal_mode = normal_mode::drawBase(state.rng);
    state.cz_cycle = {};
    state.cz_finalize = {};
    state.cz_reward = {};
    state.at_hit_stock_gained = false;
    state.last_section_flow = {};
    state.last_section_transition = {};
    state.at_window = {};
    state.bonus_cycle = {};
    state.bonus_transition = {};
    state.upper_comeback_cycle = {};
    state.at_single_transition = {};
    state.normal_at_trigger = {};
    state.normal_hit_entry = {};
    state.revival_game = {};
    state.revival_finalize = {};
}

uint32_t lever(State& state) {
    const bool cz_transition_pending =
        state.machine.area == machine_state::Area::CZ
        && !state.machine.cz.active
        && (
            pending_event::has(state.pending, pending_event::CZHit)
            || pending_event::has(state.pending, pending_event::CZThreeMissHit)
        );

    const bool at_window_pending =
        state.machine.area == machine_state::Area::AT
        && state.machine.at.active
        && state.machine.at.games_left <= 0
        && pending_event::has(state.pending, pending_event::ATWindowEmpty);

    const bool bonus_transition_pending =
        state.machine.area == machine_state::Area::Bonus
        && !state.machine.bonus.active
        && (
            pending_event::has(state.pending, pending_event::BonusComplete)
            || pending_event::has(state.pending, pending_event::BonusEpisodeUpgrade)
        );

    const bool upper_comeback_pending =
        pending_event::has(
            state.pending,
            pending_event::UpperComebackHit
        );

    const bool special_zone_transition_pending =
        pending_event::has(
            state.pending,
            pending_event::SpecialZoneAddGames
        )
        || pending_event::has(
            state.pending,
            pending_event::SpecialZoneBonus
        );

    const bool unresolved_at_pending =
        pending_event::has(state.pending, pending_event::ATHit)
        || pending_event::has(state.pending, pending_event::ATFall)
        || pending_event::has(state.pending, pending_event::ATAddGames)
        || pending_event::has(state.pending, pending_event::ATEpisode)
        || pending_event::has(state.pending, pending_event::ATUpperSpecial)
        || pending_event::has(state.pending, pending_event::ATMultiple)
        || pending_event::has(state.pending, pending_event::SectionUpperSpec);

    if (cz_transition_pending
        || at_window_pending
        || bonus_transition_pending
        || upper_comeback_pending
        || special_zone_transition_pending
        || unresolved_at_pending) {
        const auto current = state.session.lever;
        return static_cast<uint32_t>(current.role)
            | (static_cast<uint32_t>(current.special) << 8)
            | (current.main_lottery_ran ? (1u << 16) : 0u)
            | (static_cast<uint32_t>(CommandStatus::RejectedPhase) << 24);
    }

    if (!session::canLever(state.session)) {
        const auto current = state.session.lever;
        return static_cast<uint32_t>(current.role)
            | (static_cast<uint32_t>(current.special) << 8)
            | (current.main_lottery_ran ? (1u << 16) : 0u)
            | (static_cast<uint32_t>(CommandStatus::RejectedPhase) << 24);
    }

    state.at_single_transition = {};
    state.normal_at_trigger = {};
    state.special_zone_result = special_zone::HitResult::None;
    state.revival_game = {};
    state.revival_finalize = {};

    if (state.machine.area == machine_state::Area::Normal) {
        normal_state::onLever(state.machine.normal);

        state.upper_comeback_cycle =
            upper_comeback_cycle::playOne(
                state.rng,
                state.machine.upper_comeback
            );

        if (state.upper_comeback_cycle.ended
            && state.upper_comeback_cycle.hit) {
            pending_event::add(
                state.pending,
                pending_event::UpperComebackHit
            );
        }
    } else {
        state.upper_comeback_cycle = {};
    }

    const bool revival_game_active =
        state.machine.area == machine_state::Area::Revival
        && state.machine.revival.active;

    // 特殊直撃は通常時だけ。復活チャレンジは通常小役だけを同率で抽選する。
    const bool allow_special =
        state.machine.area == machine_state::Area::Normal
        && !revival_game_active;

    auto result = slotv2::lever::pull(
        state.rng,
        allow_special
    );

    if (revival_game_active) {
        state.revival_game = revival_cycle::beginGame(
            state.rng,
            state.machine.revival,
            result.role
        );
    }

    const auto special = special_result::resolve(result.special);
    const auto freeze = slotv2::freeze::begin(result.special);

    session::begin(state.session, result, special, freeze);

    const bool special_zone_game =
        result.special == SpecialHit::None
        && state.machine.area == machine_state::Area::AT
        && state.machine.at.active
        && state.machine.special_zone.active;

    if (special_zone_game) {
        state.special_zone_result = special_zone::playOne(
            state.machine.special_zone,
            state.rng
        );
        special_zone_pending::publish(
            state.special_zone_result,
            state.pending
        );
    }

    // 特化5G中は通常ATのST残Gと内部抽選を進めない。
    // 特殊直撃が割り込み中のゲームでもAT内部結果は同時確定させない。
    state.at_cycle =
        (result.special == SpecialHit::None && !special_zone_game)
        ? at_cycle::beginGame(state.rng, state.machine)
        : at_cycle::Result{};

    state.at_resolution = state.at_cycle.active
        ? at_resolution::classify(state.at_cycle.raw)
        : at_resolution::Result{};

    at_pending::publish(
        state.at_resolution,
        state.pending
    );

    if (state.at_cycle.active && state.at_cycle.window_empty_after_game) {
        pending_event::add(
            state.pending,
            pending_event::ATWindowEmpty
        );
    }

    state.at_hit_stock_gained = false;
    if (state.at_cycle.active
        && at_event::has(state.at_cycle.raw, at_event::Hit)
        && stock_lottery::onHit(state.rng)) {
        stock::add(state.machine.stock, 1u);
        state.at_hit_stock_gained = true;
    }

    state.at_window = at_window::inspect(state.machine);

    if (state.at_window.status == at_window::Status::EmptyStockAvailable) {
        pending_event::add(
            state.pending,
            pending_event::ATStockAvailable
        );
    }

    state.cz_cycle =
        (result.special == SpecialHit::None
            && state.machine.area == machine_state::Area::CZ
            && state.machine.cz.active)
        ? cz_cycle::playOne(state.rng, state.machine.cz, result.role)
        : cz_cycle::Result{};

    state.cz_finalize = cz_finalize::apply(
        state.machine,
        state.pending,
        state.cz_cycle
    );

    state.cz_reward = cz_reward::apply(
        state.rng,
        state.machine,
        state.pending,
        state.normal_mode
    );

    const bool navigation_enabled =
        state.machine.area == machine_state::Area::AT
        && state.machine.at.active;

    session::setBellNavigation(
        state.session,
        bell_navigation::make(
            state.rng,
            result.role,
            navigation_enabled
        )
    );

    state.acquisition = {};
    state.special_committed = false;
    state.last_special_apply = {};

    return static_cast<uint32_t>(result.role)
        | (static_cast<uint32_t>(result.special) << 8)
        | (result.main_lottery_ran ? (1u << 16) : 0u)
        | (static_cast<uint32_t>(result.command_status) << 24);
}

uint32_t stop(State& state, uint32_t reel, uint32_t pressed_position) {
    if (reel > 2u) {
        return (static_cast<uint32_t>(stop_shared::ResolveStatus::InvalidReel) << 16);
    }

    const auto reel_id = static_cast<ReelId>(reel);

    if (state.session.stopped[reel]) {
        return (static_cast<uint32_t>(stop_shared::ResolveStatus::InvalidReel) << 16)
            | static_cast<uint32_t>(state.session.position[reel]);
    }

    if (state.session.phase == session::Phase::SpecialPending) {
        return (static_cast<uint32_t>(stop_shared::ResolveStatus::SpecialControlPending) << 16);
    }

    if (!session::canStop(state.session, reel_id)) {
        return (static_cast<uint32_t>(stop_shared::ResolveStatus::InvalidReel) << 16);
    }

    const auto ctx = session::makeStopContext(
        state.session,
        reel_id,
        static_cast<uint8_t>(pressed_position)
    );

    const auto result = stop_controller::resolve(ctx);
    session::acceptStop(state.session, reel_id, result);

    if (state.session.stop_count == 3u) {
        state.acquisition = acquisition::judge(
            state.session.lever.role,
            state.session.position[0],
            state.session.position[1],
            state.session.position[2],
            session::hadSubstitute(state.session),
            session::hadRoleMiss(state.session),
            session::hadAssistGap(state.session)
        );

        if (state.machine.area == machine_state::Area::Normal) {
            (void)game_finalize::apply(
                state.session.lever.role,
                state.acquisition,
                state.machine.normal_progress,
                state.pending
            );

            state.normal_at_trigger =
                normal_at_trigger::applyBellFive(
                    state.machine,
                    state.pending
                );
        }

        state.at_single_transition =
            at_single_transition::apply(
                state.machine,
                state.pending,
                state.at_resolution
            );

        if (state.machine.area == machine_state::Area::Revival
            && state.revival_game.active) {
            state.revival_finalize =
                revival_cycle::finalizeGame(
                    state.machine,
                    state.machine.revival,
                    state.revival_game
                );
        }
    }

    return static_cast<uint32_t>(result.final_position)
        | (static_cast<uint32_t>(result.slip) << 8)
        | (static_cast<uint32_t>(result.status) << 16);
}

bool recordCZResult(State& state, bool hit) {
    return progress_event::onCZResolved(
        state.machine.normal_progress,
        state.pending,
        hit
    );
}

void recordNormalHit(State& state, bool was_at) {
    state.normal_at_trigger =
        normal_at_trigger::applyNextHitGuarantee(
            state.machine,
            state.pending,
            was_at
        );

    const bool effective_was_at =
        was_at || state.normal_at_trigger.started;

    progress_event::onNormalHitResolved(
        state.machine.normal_progress,
        state.pending,
        effective_was_at
    );
}

bool consumeNextHitAT(State& state) {
    return progress_event::consumeNextHitAT(
        state.machine.normal_progress,
        state.pending
    );
}

normal_hit_entry::Result resolveNormalHitAsBonus(State& state) {
    if (state.machine.area != machine_state::Area::Normal) return {};

    // 次回AT保証が立っていれば、この通常当たりをATへ変換する。
    state.normal_at_trigger =
        normal_at_trigger::applyNextHitGuarantee(
            state.machine,
            state.pending,
            false
        );

    if (state.normal_at_trigger.started) {
        progress_event::onNormalHitResolved(
            state.machine.normal_progress,
            state.pending,
            true
        );

        state.normal_hit_entry = {
            normal_hit_entry::Outcome::LowerAT,
            bonus_state::Kind::Regular
        };
        return state.normal_hit_entry;
    }

    state.normal_hit_entry = normal_hit_entry::enterBonus(
        state.machine,
        state.normal_mode
    );

    if (state.normal_hit_entry.outcome == normal_hit_entry::Outcome::Bonus) {
        progress_event::onNormalHitResolved(
            state.machine.normal_progress,
            state.pending,
            false
        );
    }

    return state.normal_hit_entry;
}

normal_hit_entry::Result resolveNormalHitAsAT(State& state) {
    if (state.machine.area != machine_state::Area::Normal) return {};

    // 既存の次回AT保証があれば、この当たりで消費だけしておく。
    state.normal_at_trigger =
        normal_at_trigger::applyNextHitGuarantee(
            state.machine,
            state.pending,
            true
        );

    state.normal_hit_entry = normal_hit_entry::enterAT(state.machine);

    if (state.normal_hit_entry.outcome == normal_hit_entry::Outcome::LowerAT) {
        progress_event::onNormalHitResolved(
            state.machine.normal_progress,
            state.pending,
            true
        );
    }

    return state.normal_hit_entry;
}

accounting::Result applyBet(State& state, int medals) {
    const auto result = accounting::debit(
        state.accounting,
        state.machine.section,
        medals
    );
    handleSection(state, result.section);
    return result;
}

accounting::Result applyPayout(State& state, int medals) {
    const auto result = accounting::credit(
        state.accounting,
        state.machine.section,
        medals
    );
    handleSection(state, result.section);
    return result;
}

void startUpperComeback(State& state) {
    upper_comeback::start(
        state.machine.upper_comeback
    );
    state.upper_comeback_cycle = {};
}

void startRevivalChallenge(
    State& state,
    at_state::Tier ended_tier
) {
    // 呼出条件は「既存の継続・ストック・引戻しを全て処理した後の完全終了」。
    at_state::end(state.machine.at);
    revival_state::start(
        state.machine.revival,
        ended_tier
    );
    state.machine.area = machine_state::Area::Revival;
    state.revival_game = {};
    state.revival_finalize = {};

    (void)pending_event::consume(
        state.pending,
        pending_event::ATWindowEmpty
    );
    (void)pending_event::consume(
        state.pending,
        pending_event::ATStockAvailable
    );
}

void recordRevivalHiddenNormalHit(State& state) {
    revival_state::recordKickedNormalHit(
        state.machine.revival
    );
}

bonus_cycle::Result applyBonusNetGain(State& state, int net_gain) {
    state.bonus_cycle = bonus_cycle::applyNetGain(
        state.rng,
        state.machine.bonus,
        net_gain
    );

    switch (state.bonus_cycle.outcome) {
        case bonus_cycle::Outcome::Completed:
            pending_event::add(
                state.pending,
                pending_event::BonusComplete
            );
            break;

        case bonus_cycle::Outcome::EpisodeUpgradePending:
            pending_event::add(
                state.pending,
                pending_event::BonusEpisodeUpgrade
            );
            break;

        case bonus_cycle::Outcome::None:
        default:
            break;
    }

    state.bonus_transition = bonus_transition::finalize(
        state.machine,
        state.pending,
        state.bonus_cycle
    );

    return state.bonus_cycle;
}

uint32_t phase(const State& state) {
    return static_cast<uint32_t>(state.session.phase);
}

uint32_t specialResult(const State& state) {
    const auto& r = state.session.special;
    return static_cast<uint32_t>(r.hit)
        | (static_cast<uint32_t>(r.target) << 8)
        | (static_cast<uint32_t>(r.stock_profile) << 16)
        | (r.freeze ? (1u << 24) : 0u);
}

uint32_t completeSpecial(State& state) {
    if (state.session.phase == session::Phase::SpecialPending
        && !state.special_committed) {
        state.last_special_apply = special_apply::apply(
            state.rng,
            state.machine,
            state.session.special
        );
        state.special_committed = true;
    }

    session::completeSpecial(state.session);
    return static_cast<uint32_t>(state.session.phase);
}

uint32_t stoppedPosition(const State& state, uint32_t reel) {
    if (reel > 2u || !state.session.stopped[reel]) return 0xffffffffu;
    return static_cast<uint32_t>(state.session.position[reel]);
}

uint32_t stopSequence(const State& state, uint32_t order_index) {
    if (order_index >= state.session.stop_count || order_index > 2u) return 0xffffffffu;
    return static_cast<uint32_t>(state.session.stop_sequence[order_index]);
}

uint32_t acquisitionPacked(const State& state) {
    return static_cast<uint32_t>(state.acquisition.status)
        | (static_cast<uint32_t>(state.acquisition.internal_role) << 8)
        | ((static_cast<uint32_t>(state.acquisition.medals) & 0xffffu) << 16);
}

uint32_t lastSpecial(const State& state) {
    return static_cast<uint32_t>(state.session.lever.special);
}

uint32_t lastRole(const State& state) {
    return static_cast<uint32_t>(state.session.lever.role);
}

uint32_t freezeActive(const State& state) {
    return state.session.freeze.active ? 1u : 0u;
}

uint32_t machineArea(const State& state) {
    return static_cast<uint32_t>(state.machine.area);
}

int64_t sectionDiff(const State& state) {
    return state.machine.section.current_diff;
}

int64_t sectionMinimum(const State& state) {
    return state.machine.section.minimum_diff;
}

uint64_t sectionCount(const State& state) {
    return state.machine.section.section_count;
}

uint32_t stockCount(const State& state) {
    return state.machine.stock.count;
}

int64_t pointCount(const State& state) {
    return state.points.points;
}

uint32_t atActive(const State& state) {
    return state.machine.at.active ? 1u : 0u;
}

uint32_t atTier(const State& state) {
    return static_cast<uint32_t>(state.machine.at.tier);
}

int32_t atGamesLeft(const State& state) {
    return state.machine.at.games_left;
}

uint32_t specialCommitted(const State& state) {
    return state.special_committed ? 1u : 0u;
}

uint32_t bellNavigationPacked(const State& state) {
    const auto& plan = state.session.bell_navigation;
    if (!plan.active) return 0u;

    // bit0 active / bits8..15 order index
    return 1u | (static_cast<uint32_t>(plan.order_index) << 8);
}

uint32_t bellNavigationNext(const State& state) {
    const auto& plan = state.session.bell_navigation;
    if (!plan.active || !state.session.navigation_correct) return 0xffffffffu;
    if (state.session.stop_count >= 3u) return 0xffffffffu;

    return static_cast<uint32_t>(
        plan.order.reel[state.session.stop_count]
    );
}

uint32_t bellNavigationCorrect(const State& state) {
    if (!state.session.bell_navigation.active) return 0u;
    return state.session.navigation_correct ? 1u : 0u;
}

uint32_t pendingEvents(const State& state) {
    return state.pending.bits;
}

uint32_t atCyclePacked(const State& state) {
    // bit0 active / bit1 hit-derived stock / bit2 window-empty / bits8..13 raw events
    return (state.at_cycle.active ? 1u : 0u)
        | (state.at_hit_stock_gained ? (1u << 1) : 0u)
        | (state.at_cycle.window_empty_after_game ? (1u << 2) : 0u)
        | ((state.at_cycle.raw.bits & 0x3fu) << 8);
}

uint32_t atResolutionPacked(const State& state) {
    // 0..7 status / 8..15 single event / 16..23 matched count
    return static_cast<uint32_t>(state.at_resolution.status)
        | (static_cast<uint32_t>(state.at_resolution.event) << 8)
        | (static_cast<uint32_t>(state.at_resolution.count) << 16);
}

uint32_t normalMode(const State& state) {
    return static_cast<uint32_t>(state.normal_mode);
}

uint32_t normalActualGames(const State& state) {
    return state.machine.normal.actual_games;
}

uint32_t normalDisplayGames(const State& state) {
    return state.machine.normal.display_games;
}

uint32_t czCyclePacked(const State& state) {
    // bit0 active / bit1 base-hit / bits8..15 games-left / bit16 ended
    return (state.cz_cycle.active ? 1u : 0u)
        | (state.cz_cycle.base_hit ? (1u << 1) : 0u)
        | (static_cast<uint32_t>(state.cz_cycle.games_left) << 8)
        | (state.cz_cycle.ended ? (1u << 16) : 0u);
}

uint32_t czFinalizePacked(const State& state) {
    // 0..7 outcome / bit8 lever-blocked
    return static_cast<uint32_t>(state.cz_finalize.outcome)
        | (state.cz_finalize.lever_blocked ? (1u << 8) : 0u);
}

uint32_t sectionRewardPacked(const State& state) {
    const auto& r = state.last_section_flow;
    // bit0 cut / bits8..15 preference / bits16..23 reward kind
    return (r.cut ? 1u : 0u)
        | (static_cast<uint32_t>(r.preference_level) << 8)
        | (static_cast<uint32_t>(r.reward.kind) << 16);
}

uint32_t sectionTransitionPacked(const State& state) {
    const auto& r = state.last_section_transition;
    // bit0 applied / bit1 tier-changed / bit2 special-started / bit3 upper-special-pending
    // 8..15 before tier / 16..23 after tier
    return (r.applied ? 1u : 0u)
        | (r.tier_changed ? (1u << 1) : 0u)
        | (r.special_started ? (1u << 2) : 0u)
        | (r.upper_special_pending ? (1u << 3) : 0u)
        | (static_cast<uint32_t>(r.before) << 8)
        | (static_cast<uint32_t>(r.after) << 16);
}

uint32_t atWindowPacked(const State& state) {
    // 0..7 status / 8..31 stock count
    return static_cast<uint32_t>(state.at_window.status)
        | ((state.at_window.stock_count & 0x00ffffffu) << 8);
}

uint32_t specialZonePacked(const State& state) {
    // bit0 active / bits8..15 games-left
    return (state.machine.special_zone.active ? 1u : 0u)
        | (static_cast<uint32_t>(state.machine.special_zone.games_left) << 8);
}

uint32_t bonusPacked(const State& state) {
    // bit0 active / bits8..15 kind / bits16..31 medals-left
    return (state.machine.bonus.active ? 1u : 0u)
        | (static_cast<uint32_t>(state.machine.bonus.kind) << 8)
        | ((static_cast<uint32_t>(state.machine.bonus.medals_left) & 0xffffu) << 16);
}

uint32_t bonusCyclePacked(const State& state) {
    const auto& r = state.bonus_cycle;
    // bit0 active-before / bits8..15 kind / bits16..23 outcome
    return (r.active_before ? 1u : 0u)
        | (static_cast<uint32_t>(r.kind) << 8)
        | (static_cast<uint32_t>(r.outcome) << 16);
}

uint32_t bonusTransitionPacked(const State& state) {
    const auto& r = state.bonus_transition;
    // 0..7 outcome / 8..15 return area
    return static_cast<uint32_t>(r.outcome)
        | (static_cast<uint32_t>(r.return_area) << 8);
}

uint32_t upperComebackPacked(const State& state) {
    const auto& current = state.machine.upper_comeback;
    const auto& last = state.upper_comeback_cycle;

    // bit0 window-active / bit1 last-ended / bit2 last-hit
    // bits8..15 current games-left / bits16..23 last games-before
    return (current.active ? 1u : 0u)
        | (last.ended ? (1u << 1) : 0u)
        | (last.hit ? (1u << 2) : 0u)
        | (static_cast<uint32_t>(current.games_left) << 8)
        | (static_cast<uint32_t>(last.games_before) << 16);
}

uint32_t atSingleTransitionPacked(const State& state) {
    return (state.at_single_transition.applied ? 1u : 0u)
        | (state.at_single_transition.regular_bonus_started ? (1u << 1) : 0u)
        | (state.at_single_transition.episode_bonus_started ? (1u << 2) : 0u)
        | (state.at_single_transition.special_started ? (1u << 3) : 0u);
}

uint32_t normalATTriggerPacked(const State& state) {
    const auto& r = state.normal_at_trigger;
    return (r.started ? 1u : 0u)
        | (r.guarantee_consumed ? (1u << 1) : 0u)
        | (r.from_bell_five ? (1u << 2) : 0u)
        | (r.from_next_hit_guarantee ? (1u << 3) : 0u);
}

uint32_t specialZoneResultPacked(const State& state) {
    return static_cast<uint32_t>(state.special_zone_result);
}

uint32_t revivalPacked(const State& state) {
    const auto& r = state.machine.revival;
    // bit0 active / bit1 hidden-normal-hit / bits8..15 games-left / bits16..23 revive tier
    return (r.active ? 1u : 0u)
        | (r.kicked_normal_hit ? (1u << 1) : 0u)
        | (static_cast<uint32_t>(r.games_left) << 8)
        | (static_cast<uint32_t>(r.revive_tier) << 16);
}

uint32_t revivalGamePacked(const State& state) {
    const auto& g = state.revival_game;
    // bit0 active / bit1 revival-hit / bits8..15 role / bits16..23 before / bits24..31 after
    return (g.active ? 1u : 0u)
        | (g.revival_hit ? (1u << 1) : 0u)
        | (static_cast<uint32_t>(g.role) << 8)
        | (static_cast<uint32_t>(g.games_before) << 16)
        | (static_cast<uint32_t>(g.games_after) << 24);
}

uint32_t revivalFinalizePacked(const State& state) {
    const auto& r = state.revival_finalize;
    // 0..7 outcome / 8..15 revived tier / bit16 stock-added
    return static_cast<uint32_t>(r.outcome)
        | (static_cast<uint32_t>(r.tier) << 8)
        | (r.stock_added ? (1u << 16) : 0u);
}

} // namespace slotv2::runtime
