#pragma once
#include <stdint.h>
#include "shared/types.hpp"
#include "reel/reel_strip.hpp"
#include "reel/stop/stop_types.hpp"

namespace slotv2::bell_shape {

inline Symbol visible(ReelId reel, uint8_t center, int row) {
    const auto strip = reel_strip::get(reel);
    if (!strip.data || strip.size == 0u) return Symbol::Unknown;
    int p = static_cast<int>(center) + row;
    while (p < 0) p += strip.size;
    p %= strip.size;
    return strip.data[p];
}

inline bool bellSymbol(ReelId reel, Symbol symbol) {
    return symbol == Symbol::Bell
        || (reel == ReelId::Right && symbol == Symbol::Red7);
}

struct Pattern { int8_t row[3]; };

inline bool rolePatterns(
    RoleFlag role,
    const Pattern*& patterns,
    uint8_t& count
) {
    static constexpr Pattern kBell9[] = {
        {{0,0,0}}     // middle: the machine's effective 9-medal line
    };
    static constexpr Pattern kBell15[] = {{{1,0,-1}}}; // ↗
    static constexpr Pattern kBell3[] = {{{-1,0,1}}};  // ↘
    switch (role) {
        case RoleFlag::Bell9:
            patterns=kBell9; count=3u; return true;
        case RoleFlag::Bell15:
            patterns=kBell15; count=1u; return true;
        case RoleFlag::ThreeMedal:
            patterns=kBell3; count=3u; return true;
        default:
            patterns=nullptr; count=0u; return false;
    }
}

inline bool candidateCompatible(
    const stop_shared::Context& ctx,
    uint8_t candidate
) {
    const Pattern* patterns=nullptr;
    uint8_t count=0u;
    if (!rolePatterns(ctx.role,patterns,count)) return false;

    for (uint8_t p=0;p<count;++p) {
        bool valid=true;
        for (uint8_t i=0;i<3u;++i) {
            const bool isCurrent = i == static_cast<uint8_t>(ctx.reel);
            if (!isCurrent && !ctx.stopped[i]) continue;
            const uint8_t center = isCurrent ? candidate : ctx.stopped_position[i];
            const auto reel = static_cast<ReelId>(i);
            if (!bellSymbol(reel,visible(reel,center,patterns[p].row[i]))) {
                valid=false; break;
            }
        }
        if (valid) return true;
    }
    return false;
}

inline bool stoppedCompatible(const stop_shared::Context& ctx) {
    const Pattern* patterns=nullptr;
    uint8_t count=0u;
    if (!rolePatterns(ctx.role,patterns,count)) return true;

    for (uint8_t p=0;p<count;++p) {
        bool valid=true;
        for (uint8_t i=0;i<3u;++i) {
            if (!ctx.stopped[i]) continue;
            const auto reel=static_cast<ReelId>(i);
            if (!bellSymbol(
                    reel,
                    visible(reel,ctx.stopped_position[i],patterns[p].row[i]))) {
                valid=false; break;
            }
        }
        if (valid) return true;
    }
    return false;
}

inline bool matchesFinal(
    RoleFlag role,
    uint8_t left,
    uint8_t middle,
    uint8_t right
) {
    const Pattern* patterns=nullptr;
    uint8_t count=0u;
    if (!rolePatterns(role,patterns,count)) return false;
    const uint8_t center[3]{left,middle,right};

    for (uint8_t p=0;p<count;++p) {
        bool valid=true;
        for (uint8_t i=0;i<3u;++i) {
            const auto reel=static_cast<ReelId>(i);
            if (!bellSymbol(
                    reel,
                    visible(reel,center[i],patterns[p].row[i]))) {
                valid=false; break;
            }
        }
        if (valid) return true;
    }
    return false;
}

} // namespace slotv2::bell_shape
