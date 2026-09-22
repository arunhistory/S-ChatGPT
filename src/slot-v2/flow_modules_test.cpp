#include <iostream>
#include "at-resolution/index.hpp"
#include "normal-mode/index.hpp"
#include "special-ceiling/index.hpp"
#include "cz-lottery/index.hpp"

int main() {
    bool ok = true;

    {
        slotv2::at_event::Result raw{};
        auto none = slotv2::at_resolution::classify(raw);
        ok = ok && none.status == slotv2::at_resolution::Status::None;

        raw.bits = slotv2::at_event::Hit;
        auto one = slotv2::at_resolution::classify(raw);
        ok = ok
            && one.status == slotv2::at_resolution::Status::Single
            && one.event == slotv2::at_resolution::Event::Hit
            && one.count == 1u;

        raw.bits = slotv2::at_event::Hit | slotv2::at_event::Fall;
        auto many = slotv2::at_resolution::classify(raw);
        ok = ok
            && many.status == slotv2::at_resolution::Status::Multiple
            && many.event == slotv2::at_resolution::Event::None
            && many.count == 2u;
    }

    {
        slotv2::Rng rng(0x11223344ULL);
        uint64_t count[4]{0,0,0,0};

        for (uint64_t i = 0; i < 1000000ULL; ++i) {
            const auto m = slotv2::normal_mode::drawBase(rng);
            const auto n = static_cast<uint8_t>(m);
            ok = ok && n < 4u;
            ++count[n];
        }

        for (int i = 0; i < 4; ++i) ok = ok && count[i] > 0;
    }

    {
        slotv2::Rng rng(0x55667788ULL);
        uint64_t g777 = 0, g1500 = 0;
        bool seen777[3]{false,false,false};

        for (uint64_t i = 0; i < 1000000ULL; ++i) {
            const auto r = slotv2::special_ceiling::draw(rng);
            if (r.ceiling == slotv2::special_ceiling::Ceiling::G1500) {
                ++g1500;
                ok = ok && r.freeze;
            } else {
                ++g777;
                const auto v = static_cast<uint8_t>(r.g777_result);
                ok = ok && v < 3u && !r.freeze;
                seen777[v] = true;
            }
        }

        ok = ok && g777 > 0 && g1500 > 0;
        ok = ok && seen777[0] && seen777[1] && seen777[2];
    }

    {
        slotv2::Rng rng(0x99AABBCCULL);
        uint64_t hits = 0;
        for (uint64_t i = 0; i < 100000ULL; ++i) {
            hits += slotv2::cz_lottery::drawBase(rng);
        }
        ok = ok && hits > 0;
    }

    if (!ok) {
        std::cerr << "slot_v2_flow_modules_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_flow_modules_test: OK\n";
    return 0;
}
