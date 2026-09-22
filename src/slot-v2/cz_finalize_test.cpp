#include <iostream>
#include "cz-finalize/index.hpp"

int main() {
    bool ok = true;

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);
        machine.area = slotv2::machine_state::Area::CZ;
        slotv2::cz_state::start(machine.cz);

        slotv2::pending_event::State pending{};
        slotv2::cz_cycle::Result hit{
            true,
            true,
            7,
            true
        };

        const auto r = slotv2::cz_finalize::apply(
            machine,
            pending,
            hit
        );

        ok = ok && r.outcome == slotv2::cz_finalize::Outcome::HitPending;
        ok = ok && r.lever_blocked;
        ok = ok && !machine.cz.active;
        ok = ok && machine.area == slotv2::machine_state::Area::CZ;
        ok = ok && slotv2::pending_event::has(
            pending,
            slotv2::pending_event::CZHit
        );
    }

    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);
        slotv2::pending_event::State pending{};

        for (int miss = 0; miss < 3; ++miss) {
            machine.area = slotv2::machine_state::Area::CZ;
            slotv2::cz_state::start(machine.cz);

            slotv2::cz_cycle::Result expired{
                true,
                false,
                0,
                true
            };

            const auto r = slotv2::cz_finalize::apply(
                machine,
                pending,
                expired
            );

            if (miss < 2) {
                ok = ok
                    && r.outcome == slotv2::cz_finalize::Outcome::MissReturnNormal
                    && !r.lever_blocked
                    && machine.area == slotv2::machine_state::Area::Normal;
            } else {
                ok = ok
                    && r.outcome == slotv2::cz_finalize::Outcome::ThreeMissHitPending
                    && r.lever_blocked
                    && machine.area == slotv2::machine_state::Area::CZ;

                ok = ok && slotv2::pending_event::has(
                    pending,
                    slotv2::pending_event::CZThreeMissHit
                );
            }
        }
    }

    if (!ok) {
        std::cerr << "slot_v2_cz_finalize_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_cz_finalize_test: OK\n";
    return 0;
}
