#include "index.hpp"

namespace slotv2::reel_strip {

static constexpr Symbol kLeft[kReelSize] = {
    Symbol::Bar,
    Symbol::Cherry,
    Symbol::Replay,
    Symbol::Bell,
    Symbol::Red7,
    Symbol::Replay,
    Symbol::Bell,
    Symbol::Replay,
    Symbol::Bell,
    Symbol::Blue7,
    Symbol::Bell,
    Symbol::Replay,
    Symbol::Watermelon,
    Symbol::Bar,
    Symbol::Cherry,
    Symbol::Bell,
    Symbol::Replay,
    Symbol::Penguin,
    Symbol::Replay,
    Symbol::Bell,
    Symbol::Watermelon
};

StripView get(ReelId reel) {
    if (reel == ReelId::Left) return {kLeft, static_cast<uint8_t>(kReelSize)};
    return {nullptr, 0};
}

} // namespace slotv2::reel_strip
