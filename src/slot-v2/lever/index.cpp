#include "index.hpp"
#include "../main-lottery/index.hpp"
#include "../special-lottery/index.hpp"

namespace slotv2::lever {

LeverResult pull(Rng& rng) {
    LeverResult result{};

    const SpecialHit special = special_lottery::draw(rng.next27());
    if (special != SpecialHit::None) {
        result.special = special;
        result.role = RoleFlag::None;
        result.main_lottery_ran = false;
        return result;
    }

    result.special = SpecialHit::None;
    result.role = main_lottery::draw(rng.next27());
    result.main_lottery_ran = true;
    return result;
}

} // namespace slotv2::lever
