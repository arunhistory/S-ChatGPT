#pragma once
#include <stdint.h>
#include "../shared/types.hpp"
#include "../stop-shared/index.hpp"
#include "../freeze/index.hpp"
#include "../special-result/index.hpp"
#include "../bell-navigation/index.hpp"

namespace slotv2::session {

enum class Phase : uint8_t {
    Idle = 0,
    Stopping = 1,
    SpecialPending = 2,
    Complete = 3
};

struct State {
    Phase phase{Phase::Idle};
    LeverResult lever{};
    special_result::Result special{};
    freeze::Directive freeze{};
    bell_navigation::Plan bell_navigation{};
    bool navigation_correct{true};
    bool stopped[3]{false,false,false};
    uint8_t position[3]{0,0,0};
    ReelId stop_sequence[3]{ReelId::Left, ReelId::Left, ReelId::Left};
    stop_shared::ResolveStatus stop_status[3]{
        stop_shared::ResolveStatus::NoLegalCandidate,
        stop_shared::ResolveStatus::NoLegalCandidate,
        stop_shared::ResolveStatus::NoLegalCandidate
    };
    uint8_t stop_count{0};
};

void reset(State& state);
bool canLever(const State& state);
bool begin(
    State& state,
    LeverResult lever,
    special_result::Result special,
    freeze::Directive freeze
);
void setBellNavigation(State& state, const bell_navigation::Plan& plan);
bool canStop(const State& state, ReelId reel);
stop_shared::Context makeStopContext(
    const State& state,
    ReelId reel,
    uint8_t pressed_position
);
void acceptStop(State& state, ReelId reel, const stop_shared::Result& result);
bool hadSubstitute(const State& state);
bool hadRoleMiss(const State& state);
bool hadAssistGap(const State& state);
void completeSpecial(State& state);

} // namespace slotv2::session
