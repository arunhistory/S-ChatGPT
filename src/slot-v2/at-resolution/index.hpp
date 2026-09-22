#pragma once
#include <stdint.h>
#include "../at-event/index.hpp"

namespace slotv2::at_resolution {

enum class Status : uint8_t {
    None = 0,
    Single = 1,
    Multiple = 2
};

enum class Event : uint8_t {
    None = 0,
    Hit = 1,
    Fall = 2,
    AddGames = 3,
    Special = 4,
    Episode = 5,
    UpperSpecial = 6
};

struct Result {
    Status status{Status::None};
    Event event{Event::None};
    uint8_t count{0};
};

// 優先順位は未確定なので勝手に決めない。
// 0件/1件/複数件だけを分類し、複数成立はそのまま保留する。
Result classify(const at_event::Result& raw);

} // namespace slotv2::at_resolution
