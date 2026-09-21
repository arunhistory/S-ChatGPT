#include "index.hpp"

namespace slotv2::reel_strip {

// ユーザー確定済みの左21コマ。未承認の再配置は行わない。
// 21→1→2 と 13→14→15 に 🍉 / BAR / 🍒 の目押し目印を保持。
static constexpr Symbol kLeft[kReelSize] = {
    Symbol::Bar,         //  1
    Symbol::Cherry,      //  2
    Symbol::Replay,      //  3
    Symbol::Bell,        //  4
    Symbol::Red7,        //  5
    Symbol::Replay,      //  6
    Symbol::Bell,        //  7
    Symbol::Replay,      //  8
    Symbol::Bell,        //  9
    Symbol::Blue7,       // 10
    Symbol::Bell,        // 11
    Symbol::Replay,      // 12
    Symbol::Watermelon,  // 13
    Symbol::Bar,         // 14
    Symbol::Cherry,      // 15
    Symbol::Bell,        // 16
    Symbol::Replay,      // 17
    Symbol::Penguin,     // 18
    Symbol::Replay,      // 19
    Symbol::Bell,        // 20
    Symbol::Watermelon   // 21
};

StripView get(ReelId reel) {
    if (reel == ReelId::Left) return {kLeft, static_cast<uint8_t>(kReelSize)};
    return {nullptr, 0};
}

} // namespace slotv2::reel_strip
