#include "index.hpp"

namespace slotv2::special_apply {

Result apply(machine_state::State& machine, const special_result::Result& special) {
    Result out{};
    out.freeze = special.freeze;

    if (special.hit == SpecialHit::None) return out;

    if (special.stock > 0u) {
        stock::add(machine.stock, special.stock);
        out.stock_added = special.stock;
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
