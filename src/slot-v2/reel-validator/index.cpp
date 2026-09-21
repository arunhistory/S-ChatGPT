#include "index.hpp"
#include "../reel-strip/index.hpp"

namespace slotv2::reel_validator {
namespace {

bool hasVisible(const reel_strip::StripView& strip, int center, Symbol symbol) {
    for (int d = -1; d <= 1; ++d) {
        int p = center + d;
        while (p < 0) p += strip.size;
        p %= strip.size;
        if (strip.data[p] == symbol) return true;
    }
    return false;
}

bool canStopSafely(const reel_strip::StripView& strip, int pressed) {
    for (int slip = 0; slip <= kMaxSlip; ++slip) {
        int p = pressed - slip;
        while (p < 0) p += strip.size;
        p %= strip.size;
        if (!hasVisible(strip, p, Symbol::Cherry)) return true;
    }
    return false;
}

bool hasBarLandmarkPair(const reel_strip::StripView& strip) {
    int pairs = 0;
    for (int bar = 0; bar < strip.size; ++bar) {
        if (strip.data[bar] != Symbol::Bar) continue;
        int cherry = bar - 1;
        int watermelon = bar - 2;
        while (cherry < 0) cherry += strip.size;
        while (watermelon < 0) watermelon += strip.size;
        cherry %= strip.size;
        watermelon %= strip.size;

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
        if (strip.data[p] == symbol && !hasVisible(strip, p, Symbol::Cherry)) return true;
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

} // namespace slotv2::reel_validator
