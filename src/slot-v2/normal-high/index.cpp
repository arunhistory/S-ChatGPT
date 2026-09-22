#include "index.hpp"

namespace slotv2::normal_high {

uint16_t entryThresholdPerThousand(RoleFlag role) {
    switch (role) {
        case RoleFlag::Bell15: return 200u;
        case RoleFlag::Watermelon: return 150u;
        case RoleFlag::WeakCherry: return 800u;
        case RoleFlag::WeakChance: return 300u;
        case RoleFlag::StrongChance: return 900u;
        case RoleFlag::Miss: return 5u;
        default: return 0u;
    }
}

uint16_t hitThresholdPerThousand(RoleFlag role) {
    switch (role) {
        case RoleFlag::Bell15: return 300u;
        case RoleFlag::Watermelon: return 200u;
        case RoleFlag::WeakCherry: return 800u;
        case RoleFlag::WeakChance: return 500u;
        case RoleFlag::StrongChance: return 990u;
        case RoleFlag::Miss: return 1u;
        default: return 0u;
    }
}

Result play(
    Rng& rng,
    State& state,
    RoleFlag role,
    uint32_t actual_games
) {
    Result out{};

    if (state.active) {
        if (state.games < 65535u) ++state.games;

        const uint16_t hit_threshold =
            hitThresholdPerThousand(role);

        if (hit_threshold > 0u
            && rng.uniformBelow(1000u) < hit_threshold) {
            const uint32_t reward_roll = rng.uniformBelow(1000u);

            if (reward_roll < 950u) {
                const uint32_t shorten_kind = rng.uniformBelow(100u);
                if (shorten_kind < 80u) {
                    out.shorten_games = normal_shortening::weak(rng);
                } else if (shorten_kind < 95u) {
                    out.shorten_games = normal_shortening::strong(rng);
                } else {
                    out.shorten_games = normal_shortening::continuous(rng);
                }
            } else if (reward_roll < 995u) {
                state.active = false;
                state.games = 0u;
                out.reward = Reward::CZ;
                out.exited = true;
                return out;
            } else {
                state.active = false;
                state.games = 0u;
                out.reward = Reward::Bonus;
                out.exited = true;
                return out;
            }
        }

        if (state.games >= 5u
            && rng.uniformBelow(10u) == 0u) {
            state.active = false;
            state.games = 0u;
            out.exited = true;
        }

        return out;
    }

    // High state unlocks after 50 actual normal games.
    if (actual_games <= 50u) return out;

    const uint16_t entry_threshold =
        entryThresholdPerThousand(role);

    if (entry_threshold > 0u
        && rng.uniformBelow(1000u) < entry_threshold) {
        state.active = true;
        state.games = 0u;
        out.entered = true;
    }

    return out;
}

void reset(State& state) {
    state = {};
}

} // namespace slotv2::normal_high
