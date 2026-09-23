#include "reel/reel_validator.hpp"
#include "reel/reel_strip.hpp"
#include "reel/stop/assist_target.hpp"

namespace slotv2::reel_validator {
namespace {

bool centerOrLowerCherry(const reel_strip::StripView& strip, int center) {
    int c = center;
    while (c < 0) c += strip.size;
    c %= strip.size;
    const int lower = (c + 1) % strip.size;
    return strip.data[c] == Symbol::Cherry
        || strip.data[lower] == Symbol::Cherry;
}

bool canStopSafely(const reel_strip::StripView& strip, int pressed) {
    for (int slip = 0; slip <= kMaxSlip; ++slip) {
        int p = pressed - slip;
        while (p < 0) p += strip.size;
        p %= strip.size;
        if (!centerOrLowerCherry(strip, p)) return true;
    }
    return false;
}

bool hasBarLandmarkPair(const reel_strip::StripView& strip) {
    int pairs = 0;
    for (int bar = 0; bar < strip.size; ++bar) {
        if (strip.data[bar] != Symbol::Bar) continue;
        int watermelon = bar - 1;
        int cherry = bar + 1;
        while (watermelon < 0) watermelon += strip.size;
        watermelon %= strip.size;
        cherry %= strip.size;

        if (strip.data[cherry] == Symbol::Cherry
            && strip.data[watermelon] == Symbol::Watermelon) {
            ++pairs;
        }
    }
    return pairs == 2;
}

bool canReachSymbolSafely(const reel_strip::StripView& strip, int pressed, Symbol symbol) {
    for (int slip = 0; slip <= kMaxSlip; ++slip) {
        int p = pressed - slip;
        while (p < 0) p += strip.size;
        p %= strip.size;
        if (strip.data[p] == symbol && !centerOrLowerCherry(strip, p)) return true;
    }
    return false;
}

}

uint32_t validateLeft() {
    const auto strip = reel_strip::get(ReelId::Left);
    if (!strip.data || strip.size != kReelSize) return 0;

    bool cherryHide = true;
    bool bell = true;
    bool replay = true;

    for (int pressed = 0; pressed < strip.size; ++pressed) {
        cherryHide = cherryHide && canStopSafely(strip, pressed);
        bell = bell && canReachSymbolSafely(strip, pressed, Symbol::Bell);
        replay = replay && canReachSymbolSafely(strip, pressed, Symbol::Replay);
    }

    uint32_t out = 0;
    if (cherryHide) out |= LeftCherryHidePossible;
    if (bell) out |= LeftBellGuaranteed;
    if (replay) out |= LeftReplayGuaranteed;
    if (hasBarLandmarkPair(strip)) out |= LeftBarLandmarkPair;
    return out;
}

uint32_t validateAssist(ReelId reel) {
    const auto strip = reel_strip::get(reel);
    if (!strip.data || strip.size != kReelSize) return 0;

    bool bell = true;
    bool replay = true;

    for (int pressed = 0; pressed < strip.size; ++pressed) {
        bool bellHere = false;
        bool replayHere = false;

        for (int slip = 0; slip <= kMaxSlip; ++slip) {
            int p = pressed - slip;
            while (p < 0) p += strip.size;
            p %= strip.size;

            const auto symbol = strip.data[p];
            const bool left_safe =
                reel != ReelId::Left || !centerOrLowerCherry(strip, p);

            bellHere = bellHere
                || (left_safe && assist_target::accepts(RoleFlag::Bell9, reel, symbol));
            replayHere = replayHere
                || (left_safe && assist_target::accepts(RoleFlag::Replay, reel, symbol));
        }

        bell = bell && bellHere;
        replay = replay && replayHere;
    }

    uint32_t out = AssistStripDefined;
    if (bell) out |= AssistBellGuaranteed;
    if (replay) out |= AssistReplayGuaranteed;
    return out;
}

uint32_t assistFailureMask(ReelId reel, RoleFlag role) {
    const auto strip = reel_strip::get(reel);
    if (!strip.data || strip.size != kReelSize) return 0x001fffffu;

    uint32_t failures = 0u;

    for (int pressed = 0; pressed < strip.size; ++pressed) {
        bool found = false;

        for (int slip = 0; slip <= kMaxSlip; ++slip) {
            int p = pressed - slip;
            while (p < 0) p += strip.size;
            p %= strip.size;

            if (!assist_target::accepts(role, reel, strip.data[p])) {
                continue;
            }

            if (reel == ReelId::Left && centerOrLowerCherry(strip, p)) {
                continue;
            }

            found = true;
            break;
        }

        if (!found) failures |= (1u << pressed);
    }

    return failures;
}

uint32_t readyMask() {
    uint32_t mask = 0;
    for (uint8_t i = 0; i < 3u; ++i) {
        const auto strip = reel_strip::get(static_cast<ReelId>(i));
        if (strip.data && strip.size == kReelSize) {
            mask |= (1u << i);
        }
    }
    return mask;
}

} // namespace slotv2::reel_validator
