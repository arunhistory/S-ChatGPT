#include "core/lever.hpp"
#include "core/main_lottery.hpp"
#include "special/special_lottery.hpp"

namespace slotv2::lever {

LeverResult pull(Rng& rng, bool allow_special) {
    LeverResult result{};
    result.command_status = CommandStatus::Ok;

    const SpecialHit special = allow_special
        ? special_lottery::draw(rng.next27())
        : SpecialHit::None;

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
