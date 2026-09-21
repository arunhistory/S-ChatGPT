#include "index.hpp"

namespace slotv2::at_lottery {

Draw drawBase(Rng& rng) {
    Draw out{};
    out.hit = rng.oneIn(200u);
    out.fall = rng.oneIn(400u);
    out.add_games = rng.oneIn(300u);
    out.special = rng.oneIn(700u);
    out.episode = rng.oneIn(1000u);
    out.upper_special = rng.oneIn(5000u);
    return out;
}

} // namespace slotv2::at_lottery
