#include "index.hpp"
#include "../shared/types.hpp"

namespace slotv2::at_lottery {
namespace {

struct Rates {
    uint32_t hit;
    uint32_t fall;
    uint32_t add_games;
    uint32_t special;
    uint32_t episode;
    uint32_t upper_special;
};

constexpr uint32_t countFor(uint32_t denominator) {
    return denominator == 0u ? 0u : (kRngSpace / denominator);
}

Rates ratesFor(const at_state::State& state) {
    const bool higher = state.tier != at_state::Tier::Lower;

    switch (state.table) {
        case at_state::Table::Heaven:
            return higher
                ? Rates{48u, 400u, 145u, 340u, 600u, 0u}
                : Rates{100u, 400u, 300u, 700u, 800u, 0u};

        case at_state::Table::SuperHeaven:
            return higher
                ? Rates{95u, 400u, 60u, 340u, 500u, 0u}
                : Rates{200u, 400u, 125u, 700u, 700u, 0u};

        case at_state::Table::Specialized:
            return higher
                ? Rates{170u, 400u, 260u, 120u, 0u, 950u}
                : Rates{350u, 400u, 550u, 250u, 0u, 2000u};

        case at_state::Table::Normal:
        default:
            return higher
                ? Rates{95u, 400u, 145u, 340u, 0u, 0u}
                : Rates{200u, 400u, 300u, 700u, 0u, 0u};
    }
}

bool take(uint32_t& roll, uint32_t count) {
    if (count == 0u) return false;
    if (roll < count) return true;
    roll -= count;
    return false;
}

} // namespace

Draw draw(Rng& rng, const at_state::State& state) {
    const auto rates = ratesFor(state);

    uint32_t roll = static_cast<uint32_t>(rng.next64()) & (kRngSpace - 1u);
    Draw out{};

    if (take(roll, countFor(rates.hit))) {
        out.hit = true;
        return out;
    }
    if (take(roll, countFor(rates.fall))) {
        out.fall = true;
        return out;
    }
    if (take(roll, countFor(rates.add_games))) {
        out.add_games = true;
        return out;
    }
    if (take(roll, countFor(rates.special))) {
        out.special = true;
        return out;
    }
    if (take(roll, countFor(rates.episode))) {
        out.episode = true;
        return out;
    }
    if (take(roll, countFor(rates.upper_special))) {
        out.upper_special = true;
        return out;
    }

    return out;
}

uint8_t count(const Draw& draw) {
    return static_cast<uint8_t>(
        (draw.hit ? 1u : 0u)
        + (draw.fall ? 1u : 0u)
        + (draw.add_games ? 1u : 0u)
        + (draw.special ? 1u : 0u)
        + (draw.episode ? 1u : 0u)
        + (draw.upper_special ? 1u : 0u)
    );
}

} // namespace slotv2::at_lottery
