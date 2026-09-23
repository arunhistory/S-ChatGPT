#include "reel/reel_strip.hpp"

namespace slotv2::reel_strip {

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

static constexpr Symbol kMiddle[kReelSize] = {
    Symbol::Bell,        //  1
    Symbol::Bar,         //  2
    Symbol::Replay,      //  3
    Symbol::Watermelon,  //  4
    Symbol::Penguin,     //  5
    Symbol::Bell,        //  6
    Symbol::Replay,      //  7
    Symbol::Bell,        //  8
    Symbol::Red7,        //  9
    Symbol::Cherry,      // 10
    Symbol::Replay,      // 11
    Symbol::Penguin,     // 12
    Symbol::Bell,        // 13
    Symbol::Replay,      // 14
    Symbol::Blue7,       // 15
    Symbol::Bar,         // 16
    Symbol::Bell,        // 17
    Symbol::Watermelon,  // 18
    Symbol::Replay,      // 19
    Symbol::Bell,        // 20
    Symbol::Replay       // 21
};

static constexpr Symbol kRight[kReelSize] = {
    Symbol::Replay,      //  1
    Symbol::Bell,        //  2
    Symbol::Penguin,     //  3
    Symbol::Snow,        //  4
    Symbol::Replay,      //  5
    Symbol::Watermelon,  //  6
    Symbol::Bell,        //  7
    Symbol::Penguin,     //  8
    Symbol::Replay,      //  9
    Symbol::Snow,        // 10
    Symbol::Blue7,       // 11
    Symbol::Red7,        // 12 (bell substitute only while bell is internally active)
    Symbol::Bar,         // 13
    Symbol::Replay,      // 14
    Symbol::Snow,        // 15
    Symbol::Bell,        // 16
    Symbol::Penguin,     // 17
    Symbol::Replay,      // 18
    Symbol::Bar,         // 19
    Symbol::Bell,        // 20
    Symbol::Replay       // 21
};

StripView get(ReelId reel) {
    switch (reel) {
        case ReelId::Left:
            return {kLeft, static_cast<uint8_t>(kReelSize)};
        case ReelId::Middle:
            return {kMiddle, static_cast<uint8_t>(kReelSize)};
        case ReelId::Right:
            return {kRight, static_cast<uint8_t>(kReelSize)};
    }
    return {nullptr, 0};
}

} // namespace slotv2::reel_strip
