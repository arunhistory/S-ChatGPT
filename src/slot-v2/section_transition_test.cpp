#include <iostream>
#include "section-transition/index.hpp"
#include "at-window/index.hpp"

int main() {
    bool ok = true;

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);
        slotv2::at_state::start(
            machine.at,
            slotv2::at_state::Tier::Lower
        );
        machine.area = slotv2::machine_state::Area::AT;

        slotv2::pending_event::State pending{};
        slotv2::pending_event::add(
            pending,
            slotv2::pending_event::SectionTierUp
        );

        slotv2::section_flow::Result flow{};
        flow.cut = true;
        flow.reward.kind = slotv2::section_reward::Kind::TierUp;

        const auto first = slotv2::section_transition::apply(
            machine,
            pending,
            flow
        );

        ok = ok && first.applied && first.tier_changed;
        ok = ok && machine.at.tier == slotv2::at_state::Tier::Middle;

        slotv2::pending_event::add(
            pending,
            slotv2::pending_event::SectionTierUp
        );

        const auto second = slotv2::section_transition::apply(
            machine,
            pending,
            flow
        );

        ok = ok && second.applied && second.tier_changed;
        ok = ok && machine.at.tier == slotv2::at_state::Tier::Upper;
    }

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);
        slotv2::at_state::start(
            machine.at,
            slotv2::at_state::Tier::Upper
        );
        machine.area = slotv2::machine_state::Area::AT;

        slotv2::pending_event::State pending{};
        slotv2::pending_event::add(
            pending,
            slotv2::pending_event::SectionSpecial
        );

        slotv2::section_flow::Result flow{};
        flow.cut = true;
        flow.reward.kind = slotv2::section_reward::Kind::Special;

        const auto r = slotv2::section_transition::apply(
            machine,
            pending,
            flow
        );

        ok = ok && r.applied && r.special_started;
        ok = ok && machine.special_zone.active;
        ok = ok && machine.special_zone.games_left == 5u;
    }

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);
        slotv2::at_state::start(
            machine.at,
            slotv2::at_state::Tier::Middle
        );
        machine.area = slotv2::machine_state::Area::AT;

        machine.at.games_left = 0;

        auto noStock = slotv2::at_window::inspect(machine);
        ok = ok
            && noStock.status == slotv2::at_window::Status::EmptyNoStock;

        slotv2::stock::add(machine.stock, 3u);
        auto withStock = slotv2::at_window::inspect(machine);
        ok = ok
            && withStock.status == slotv2::at_window::Status::EmptyStockAvailable
            && withStock.stock_count == 3u;
    }

    if (!ok) {
        std::cerr << "slot_v2_section_transition_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_section_transition_test: OK\n";
    return 0;
}
