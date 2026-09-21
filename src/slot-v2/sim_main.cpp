#include <cstdlib>
#include <iostream>
#include "shared/rng.hpp"
#include "lever/index.hpp"

int main(int argc, char** argv) {
    long long spins = 1000000;
    if (argc > 1) spins = std::atoll(argv[1]);

    slotv2::Rng rng(0x5343484154475054ULL);
    long long special8192 = 0;
    long long special32768 = 0;
    long long freeze = 0;
    long long main = 0;

    for (long long i = 0; i < spins; ++i) {
        const auto r = slotv2::lever::pull(rng);
        switch (r.special) {
            case slotv2::SpecialHit::MiddleATStock: ++special8192; break;
            case slotv2::SpecialHit::UpperAT: ++special32768; break;
            case slotv2::SpecialHit::Freeze: ++freeze; break;
            case slotv2::SpecialHit::None: ++main; break;
        }
    }

    std::cout
        << "spins=" << spins << '\n'
        << "main=" << main << '\n'
        << "special_8192=" << special8192 << '\n'
        << "special_32768=" << special32768 << '\n'
        << "freeze=" << freeze << '\n';
}
