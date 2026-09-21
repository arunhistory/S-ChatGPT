#include "index.hpp"

namespace slotv2::reel_strip {

// 左リール21コマ。
// BARを目印に🍉/🍒をフォローできるよう、2か所とも
// 「🍉 → 🍒 → BAR」の3コマを近接配置。
// 同時に、非チェリー成立時の🍒蹴りと、🔔/REPLAYの0〜4コマ
// 引き込み保証が両立する配置へ調整している。
static constexpr Symbol kLeft[kReelSize] = {
    Symbol::Replay,      //  1
    Symbol::Watermelon,  //  2
    Symbol::Cherry,      //  3
    Symbol::Bar,         //  4
    Symbol::Bell,        //  5
    Symbol::Replay,      //  6
    Symbol::Replay,      //  7
    Symbol::Bell,        //  8
    Symbol::Watermelon,  //  9
    Symbol::Cherry,      // 10
    Symbol::Bar,         // 11
    Symbol::Replay,      // 12
    Symbol::Bell,        // 13
    Symbol::Bell,        // 14
    Symbol::Replay,      // 15
    Symbol::Blue7,       // 16
    Symbol::Penguin,     // 17
    Symbol::Bell,        // 18
    Symbol::Replay,      // 19
    Symbol::Red7,        // 20
    Symbol::Bell         // 21
};

StripView get(ReelId reel) {
    if (reel == ReelId::Left) return {kLeft, static_cast<uint8_t>(kReelSize)};
    return {nullptr, 0};
}

} // namespace slotv2::reel_strip
