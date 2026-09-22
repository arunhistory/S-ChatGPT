#include "index.hpp"

namespace slotv2::normal_route {
namespace {

constexpr uint16_t kNormalA[10] = {
    500, 700, 750, 900, 1000, 1100, 1250, 1350, 1450, 1500
};
constexpr uint16_t kNormalB[10] = {
    250, 300, 400, 500, 600, 700, 750, 800, 1000, 1250
};
constexpr uint16_t kHeaven[10] = {
    100, 200, 250, 300, 400, 500, 600, 700, 750, 750
};
constexpr uint16_t kSuperHeaven[10] = {
    50, 50, 100, 100, 200, 200, 250, 250, 300, 300
};

}

uint16_t ceilingFor(
    normal_mode::Mode mode,
    uint8_t pattern
) {
    const uint8_t p = static_cast<uint8_t>(pattern % 10u);

    switch (mode) {
        case normal_mode::Mode::NormalA:
            return kNormalA[p];
        case normal_mode::Mode::NormalB:
            return kNormalB[p];
        case normal_mode::Mode::Heaven:
            return kHeaven[p];
        case normal_mode::Mode::SuperHeaven:
            return kSuperHeaven[p];
        case normal_mode::Mode::Special:
            // Special ceiling is chosen separately at entry.
            return 0u;
    }
    return 0u;
}

void rerollBase(
    Rng& rng,
    normal_mode::Mode& mode,
    State& state
) {
    mode = normal_mode::drawBase(rng);
    state.pattern = static_cast<uint8_t>(rng.uniformBelow(10u));
    state.ceiling = ceilingFor(mode, state.pattern);
    state.special_window_checked = false;
}

bool checkSpecialWindow(
    Rng& rng,
    normal_mode::Mode& mode,
    State& state,
    uint32_t actual_games
) {
    if (mode != normal_mode::Mode::SuperHeaven) return false;
    if (state.special_window_checked) return false;
    if (actual_games < 20u) return false;

    state.special_window_checked = true;

    if (rng.uniformBelow(10u) != 0u) {
        return false;
    }

    mode = normal_mode::Mode::Special;
    state.pattern = static_cast<uint8_t>(rng.uniformBelow(10u));

    const auto special = special_ceiling::draw(rng);
    state.ceiling = static_cast<uint16_t>(special.ceiling);
    return true;
}

bool reached(
    const State& state,
    uint32_t display_games
) {
    return state.ceiling > 0u
        && display_games >= state.ceiling;
}

} // namespace slotv2::normal_route
