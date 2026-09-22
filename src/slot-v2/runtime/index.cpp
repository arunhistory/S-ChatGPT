#include "index.hpp"
#include "../lever/index.hpp"
#include "../freeze/index.hpp"
#include "../special-result/index.hpp"
#include "../stop-controller/index.hpp"

namespace slotv2::runtime {

void reset(State& state, uint64_t seed) {
    state.rng.reset(seed);
    session::reset(state.session);
    state.acquisition = {};
    machine_state::reset(state.machine);
    state.accounting = {};
    state.points = {};
    state.special_committed = false;
    state.last_special_apply = {};
}

uint32_t lever(State& state) {
    if (!session::canLever(state.session)) {
        const auto current = state.session.lever;
        return static_cast<uint32_t>(current.role)
            | (static_cast<uint32_t>(current.special) << 8)
            | (current.main_lottery_ran ? (1u << 16) : 0u)
            | (static_cast<uint32_t>(CommandStatus::RejectedPhase) << 24);
    }

    auto result = slotv2::lever::pull(state.rng);
    const auto special = special_result::resolve(result.special);
    const auto freeze = slotv2::freeze::begin(result.special);

    session::begin(state.session, result, special, freeze);
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
    }

    return static_cast<uint32_t>(result.final_position)
        | (static_cast<uint32_t>(result.slip) << 8)
        | (static_cast<uint32_t>(result.status) << 16);
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

} // namespace slotv2::runtime
