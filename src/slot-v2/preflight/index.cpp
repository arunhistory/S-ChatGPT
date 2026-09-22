#include "index.hpp"
#include "../reel-validator/index.hpp"

namespace slotv2::preflight {

uint32_t check() {
    uint32_t out = 0;

    const uint32_t ready = reel_validator::readyMask();
    if (ready & (1u << 0)) out |= LeftDefined;
    if (ready & (1u << 1)) out |= MiddleDefined;
    if (ready & (1u << 2)) out |= RightDefined;

    const uint32_t left = reel_validator::validateLeft();
    if (left & reel_validator::LeftCherryHidePossible) out |= LeftCherryHide;
    if (left & reel_validator::LeftBellGuaranteed) out |= LeftBellAssist;
    if (left & reel_validator::LeftReplayGuaranteed) out |= LeftReplayAssist;
    if (left & reel_validator::LeftBarLandmarkPair) out |= LeftBarLandmarks;

    const uint32_t middle = reel_validator::validateAssist(ReelId::Middle);
    if (middle & reel_validator::AssistBellGuaranteed) out |= MiddleBellAssist;
    if (middle & reel_validator::AssistReplayGuaranteed) out |= MiddleReplayAssist;

    const uint32_t right = reel_validator::validateAssist(ReelId::Right);
    if (right & reel_validator::AssistBellGuaranteed) out |= RightBellAssist;
    if (right & reel_validator::AssistReplayGuaranteed) out |= RightReplayAssist;

    const uint32_t required =
        LeftDefined |
        MiddleDefined |
        RightDefined |
        LeftCherryHide |
        LeftBellAssist |
        LeftReplayAssist |
        LeftBarLandmarks |
        MiddleBellAssist |
        MiddleReplayAssist |
        RightBellAssist |
        RightReplayAssist;

    if ((out & required) == required) out |= Playable;
    return out;
}

} // namespace slotv2::preflight
