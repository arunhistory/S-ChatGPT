#include "at/at_event.hpp"

namespace slotv2::at_event {

Result fromDraw(const at_lottery::Draw& draw) {
    Result out{};

    if (draw.hit) out.bits |= Hit;
    if (draw.fall) out.bits |= Fall;
    if (draw.add_games) out.bits |= AddGames;
    if (draw.special) out.bits |= Special;
    if (draw.episode) out.bits |= Episode;
    if (draw.upper_special) out.bits |= UpperSpecial;

    return out;
}

bool has(const Result& result, Bits bit) {
    return (result.bits & static_cast<uint32_t>(bit)) != 0u;
}

} // namespace slotv2::at_event
