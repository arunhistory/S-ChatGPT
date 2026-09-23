#include <iostream>
#include "normal/normal_route.hpp"
#include "normal/normal_ceiling.hpp"

int main() {
    bool ok = true;

    using slotv2::normal_mode::Mode;

    ok = ok && slotv2::normal_route::ceilingFor(Mode::NormalA, 0u) == 500u;
    ok = ok && slotv2::normal_route::ceilingFor(Mode::NormalA, 9u) == 1500u;
    ok = ok && slotv2::normal_route::ceilingFor(Mode::NormalB, 0u) == 250u;
    ok = ok && slotv2::normal_route::ceilingFor(Mode::Heaven, 9u) == 750u;
    ok = ok && slotv2::normal_route::ceilingFor(Mode::SuperHeaven, 0u) == 50u;
    ok = ok && slotv2::normal_route::ceilingFor(Mode::SuperHeaven, 9u) == 300u;

    uint32_t cz = 0u;
    uint32_t bonus = 0u;
    uint32_t at = 0u;
    for (uint8_t i = 0; i < 100u; ++i) {
        const auto r = slotv2::normal_ceiling::fromRoll(
            Mode::NormalA,
            500u,
            i
        );
        if (r == slotv2::normal_ceiling::Reward::CZ) ++cz;
        else if (r == slotv2::normal_ceiling::Reward::Bonus) ++bonus;
        else if (r == slotv2::normal_ceiling::Reward::LowerAT) ++at;
    }

    ok = ok && cz == 70u && bonus == 25u && at == 5u;

    ok = ok && slotv2::normal_ceiling::fromRoll(
        Mode::Special,
        777u,
        0u
    ) == slotv2::normal_ceiling::Reward::LowerATWithStock;

    ok = ok && slotv2::normal_ceiling::fromRoll(
        Mode::Special,
        1500u,
        0u
    ) == slotv2::normal_ceiling::Reward::Freeze;

    if (!ok) {
        std::cerr << "slot_v2_normal_route_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_normal_route_test: OK\n";
    return 0;
}
