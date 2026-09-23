#include "special/special_apply.hpp"
#include "stock/stock_count_lottery.hpp"
#include "at/at_cold.hpp"

namespace slotv2::special_apply {

Result apply(
    Rng& rng,
    machine_state::State& machine,
    const special_result::Result& special
) {
    Result out{};
    out.freeze = special.freeze;

    if (special.hit == SpecialHit::None) return out;

    uint8_t stock_count = 0u;
    if (special.stock_profile != stock_count_lottery::Profile::None) {
        stock_count = stock_count_lottery::draw(
            rng,
            special.stock_profile
        );
        out.stock_added = stock_count;
    }

    // Freeze already has its forced blue-777 start signal, so it enters
    // Upper AT immediately after that sequence completes.
    if (special.freeze) {
        if (stock_count > 0u) {
            stock::add(machine.stock, stock_count);
        }
        at_state::start(machine.at, at_state::Tier::Upper);
        at_cold::reroll(rng, machine.at);
        machine.area = machine_state::Area::AT;
        out.applied = true;
        out.at_started = true;
        out.tier = at_state::Tier::Upper;
        return out;
    }

    switch (special.target) {
        case special_result::EntryTarget::MiddleAT:
            entry_gate::queueAT(
                machine.entry_gate,
                at_state::Tier::Middle,
                stock_count
            );
            out.applied = true;
            out.at_started = false;
            out.tier = at_state::Tier::Middle;
            return out;

        case special_result::EntryTarget::UpperAT:
            entry_gate::queueAT(
                machine.entry_gate,
                at_state::Tier::Upper,
                stock_count
            );
            out.applied = true;
            out.at_started = false;
            out.tier = at_state::Tier::Upper;
            return out;

        case special_result::EntryTarget::None:
        default:
            return out;
    }
}

} // namespace slotv2::special_apply
