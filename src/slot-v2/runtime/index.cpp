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
#include "../section-transition/index.hpp"
#include "../at-window/index.hpp"

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
    state.at_hit_stock_gained = false;
    state.last_section_flow = {};
    state.last_section_transition = {};
    state.at_window = {};
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

    if (cz_transition_pending || at_window_pending) {
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

    if (state.machine.area == machine_state::Area::Normal) {
        normal_state::onLever(state.machine.normal);
    }

    auto result = slotv2::lever::pull(state.rng);
    const auto special = special_result::resolve(result.special);
    const auto freeze = slotv2::freeze::begin(result.special);

    session::begin(state.session, result, special, freeze);

    // 特殊直撃が割り込み中のゲームではAT内部結果を同時確定させない。
    // 通常ATゲームだけ、独立抽選の生bitを保持する。
    state.at_cycle = result.special == SpecialHit::None
        ? at_cycle::beginGame(state.rng, state.machine)
        : at_cycle::Result{};

    state.at_resolution = state.at_cycle.active
        ? at_resolution::classify(state.at_cycle.raw)
        : at_resolution::Result{};

    at_pending::publish(
        state.at_resolution,
        state.pending
    );

    state.at_window = at_window::inspect(state.machine);

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

    state.cz_cycle =
        (result.special == SpecialHit::None
            && state.machine.area == machine_state::Area::CZ
            && state.machine.cz.active)
        ? cz_cycle::playOne(state.rng, state.machine.cz)
        : cz_cycle::Result{};

    state.cz_finalize = cz_finalize::apply(
        state.machine,
        state.pending,
        state.cz_cycle
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

        (void)game_finalize::apply(
            state.session.lever.role,
            state.acquisition,
            state.machine.normal_progress,
            state.pending
        );
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
    progress_event::onNormalHitResolved(
        state.machine.normal_progress,
        state.pending,
        was_at
    );
}

bool consumeNextHitAT(State& state) {
    return progress_event::consumeNextHitAT(
        state.machine.normal_progress,
        state.pending
    );
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

} // namespace slotv2::runtime
