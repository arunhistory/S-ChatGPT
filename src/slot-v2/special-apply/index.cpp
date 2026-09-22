#include "index.hpp"
#include "../stock-count-lottery/index.hpp"

namespace slotv2::special_apply {

Result apply(
    Rng& rng,
    machine_state::State& machine,
    const special_result::Result& special
) {
    Result out{};
    out.freeze = special.freeze;

    if (special.hit == SpecialHit::None) return out;

    if (special.stock_profile != stock_count_lottery::Profile::None) {
        const uint8_t stock_count = stock_count_lottery::draw(
            rng,
            special.stock_profile
        );
        stock::add(machine.stock, stock_count);
        out.stock_added = stock_count;
    }

    switch (special.target) {
        case special_result::EntryTarget::MiddleAT:
            at_state::start(machine.at, at_state::Tier::Middle);
            machine.area = machine_state::Area::AT;
            out.applied = true;
            out.at_started = true;
            out.tier = at_state::Tier::Middle;
            return out;

        case special_result::EntryTarget::UpperAT:
            at_state::start(machine.at, at_state::Tier::Upper);
            machine.area = machine_state::Area::AT;
            out.applied = true;
            out.at_started = true;
            out.tier = at_state::Tier::Upper;
            return out;

        case special_result::EntryTarget::None:
        default:
            out.applied = out.stock_added > 0u;
            return out;
    }
}

} // namespace slotv2::special_apply
