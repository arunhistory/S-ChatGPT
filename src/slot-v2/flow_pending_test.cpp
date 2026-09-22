#include <iostream>
#include "at-pending/index.hpp"
#include "section-flow/index.hpp"

int main() {
    bool ok = true;

    {
        slotv2::pending_event::State p{};

        slotv2::at_resolution::Result single{
            slotv2::at_resolution::Status::Single,
            slotv2::at_resolution::Event::Episode,
            1
        };

        slotv2::at_pending::publish(single, p);
        ok = ok && slotv2::pending_event::has(
            p,
            slotv2::pending_event::ATEpisode
        );

        slotv2::at_resolution::Result multiple{
            slotv2::at_resolution::Status::Multiple,
            slotv2::at_resolution::Event::None,
            2
        };

        slotv2::at_pending::publish(multiple, p);
        ok = ok && slotv2::pending_event::has(
            p,
            slotv2::pending_event::ATMultiple
        );
    }

    {
        slotv2::Rng rng(0xABCD1234ULL);
        slotv2::stock::State stocks{};
        slotv2::stock::add(stocks, 5);

        slotv2::section::ApplyResult noCut{};
        auto n = slotv2::section_flow::onSectionApply(
            rng,
            noCut,
            stocks,
            slotv2::at_state::Tier::Lower
        );
        ok = ok && !n.cut && stocks.count == 5u;

        slotv2::section::ApplyResult cut{};
        cut.cut = true;

        auto r = slotv2::section_flow::onSectionApply(
            rng,
            cut,
            stocks,
            slotv2::at_state::Tier::Lower
        );

        ok = ok && r.cut;
        ok = ok && r.preference_level == 3u;
        ok = ok && stocks.count == 0u;
    }

    if (!ok) {
        std::cerr << "slot_v2_flow_pending_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_flow_pending_test: OK\n";
    return 0;
}
