#include "at/at_lottery.hpp"
#include "shared/types.hpp"
#include "at/at_cold.hpp"

namespace slotv2::at_lottery {
namespace {

struct Rates {
    uint32_t hit, fall, add_games, special, episode, upper_special;
};

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

uint32_t normalized(uint32_t denominator, uint32_t scale_ppm, bool cold) {
    if (denominator == 0u) return 0u;
    const uint64_t base = kRngSpace / denominator;
    const uint32_t scaled = static_cast<uint32_t>(
        (base * scale_ppm + 500000u) / 1000000u
    );
    return at_cold::scaleGrowthCount(scaled, cold);
}

bool take(uint32_t& roll, uint32_t count) {
    if (roll < count) return true;
    roll -= count;
    return false;
}

} // namespace

Regions regionsFor(const at_state::State& state) {
    const auto rates = ratesFor(state);
    // Scale every non-Fall ordinary event proportionally across the four
    // table-specific distributions. Stationary table mixture:
    // Normal/Heaven/SuperHeaven/Specialized = 7/16,4/16,1/16,4/16.
    // The normalization assumes 60% cold and 70% cold growth (=0.82 mean).
    // 1/80 lower and 1/50 middle/upper are statistical targets, not
    // per-table or per-session guarantees.
    const uint32_t scale_ppm = state.tier == at_state::Tier::Lower
        ? 1145794u
        : (state.tier == at_state::Tier::Middle
            ? 932185u : 920504u);
    const uint32_t chain_den = state.tier == at_state::Tier::Lower
        ? 500u
        : (state.tier == at_state::Tier::Middle ? 450u : 400u);

    return {
        normalized(rates.hit, scale_ppm, state.cold),
        kRngSpace / 400u,
        normalized(rates.add_games, scale_ppm, state.cold),
        normalized(rates.special, scale_ppm, state.cold),
        normalized(rates.episode, scale_ppm, state.cold),
        normalized(rates.upper_special, scale_ppm, state.cold),
        normalized(chain_den, 1000000u, state.cold)
    };
}

Draw draw(Rng& rng, const at_state::State& state) {
    const auto regions = regionsFor(state);
    uint32_t roll = static_cast<uint32_t>(rng.next64()) & (kRngSpace - 1u);
    Draw out{};

    if (take(roll, regions.hit)) out.hit = true;
    else if (take(roll, regions.fall)) out.fall = true;
    else if (take(roll, regions.add_games)) out.add_games = true;
    else if (take(roll, regions.special)) out.special = true;
    else if (take(roll, regions.episode)) out.episode = true;
    else if (take(roll, regions.upper_special)) out.upper_special = true;
    else if (take(roll, regions.chain_zone)) out.chain_zone = true;

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
        + (draw.chain_zone ? 1u : 0u)
    );
}

} // namespace slotv2::at_lottery
