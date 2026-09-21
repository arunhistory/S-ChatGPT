#include "index.hpp"

namespace slotv2::reel_candidate {

static constexpr Symbol kMiddle[kReelSize] = {
    Symbol::Bell,        //  1
    Symbol::Replay,      //  2
    Symbol::Bell,        //  3
    Symbol::Red7,        //  4
    Symbol::Watermelon,  //  5
    Symbol::Replay,      //  6
    Symbol::Bar,         //  7
    Symbol::Bell,        //  8
    Symbol::Penguin,     //  9
    Symbol::Replay,      // 10
    Symbol::Blue7,       // 11
    Symbol::Bell,        // 12
    Symbol::Cherry,      // 13
    Symbol::Replay,      // 14
    Symbol::Bar,         // 15
    Symbol::Watermelon,  // 16
    Symbol::Bell,        // 17
    Symbol::Replay,      // 18
    Symbol::Bell,        // 19
    Symbol::Replay,      // 20
    Symbol::Snow         // 21
};

static constexpr Symbol kRight[kReelSize] = {
    Symbol::Replay,      //  1
    Symbol::Snow,        //  2
    Symbol::Bell,        //  3
    Symbol::Penguin,     //  4
    Symbol::Replay,      //  5
    Symbol::Bar,         //  6
    Symbol::Bell,        //  7
    Symbol::Replay,      //  8
    Symbol::Watermelon,  //  9
    Symbol::Replay,      // 10
    Symbol::Blue7,       // 11
    Symbol::Red7,        // 12
    Symbol::Bar,         // 13
    Symbol::Replay,      // 14
    Symbol::Snow,        // 15
    Symbol::Bell,        // 16
    Symbol::Replay,      // 17
    Symbol::Bar,         // 18
    Symbol::Snow,        // 19
    Symbol::Replay,      // 20
    Symbol::Bell         // 21
};

reel_strip::StripView middle() {
    return {kMiddle, static_cast<uint8_t>(kReelSize)};
}

reel_strip::StripView right() {
    return {kRight, static_cast<uint8_t>(kReelSize)};
}

} // namespace slotv2::reel_candidate
