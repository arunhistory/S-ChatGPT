#include "reel/stop/rules/common_rule.hpp"

namespace slotv2::stop_rules {

bool visibleHas(const reel_strip::StripView& strip, uint8_t center, Symbol symbol) {
    if (!strip.data || strip.size == 0) return false;
    const int n = strip.size;
    for (int row = -1; row <= 1; ++row) {
        int p = static_cast<int>(center) + row;
        while (p < 0) p += n;
        p %= n;
        if (strip.data[p] == symbol) return true;
    }
    return false;
}

bool centerIs(const reel_strip::StripView& strip, uint8_t center, Symbol symbol) {
    return strip.data && strip.size && strip.data[center % strip.size] == symbol;
}

bool leftCherrySafe(
    const stop_shared::Context& ctx,
    const reel_strip::StripView& strip,
    uint8_t candidate
) {
    if (ctx.reel != ReelId::Left) return true;
    if (ctx.role == RoleFlag::WeakCherry || ctx.role == RoleFlag::StrongCherry) {
        return true;
    }
    if (!strip.data || strip.size == 0) return false;

    // Non-cherry roles may show cherry on the top row, but never on
    // the middle or lower row where it would form the cherry result.
    const uint8_t center = static_cast<uint8_t>(candidate % strip.size);
    const uint8_t lower = static_cast<uint8_t>((center + 1u) % strip.size);

    return strip.data[center] != Symbol::Cherry
        && strip.data[lower] != Symbol::Cherry;
}

bool completesReservedLine(
    const stop_shared::Context& ctx,
    const reel_strip::StripView& strip,
    uint8_t candidate
) {
    Symbol symbol[3]{Symbol::Unknown, Symbol::Unknown, Symbol::Unknown};
    bool ready[3]{false, false, false};

    for (uint8_t i = 0; i < 3u; ++i) {
        if (!ctx.stopped[i]) continue;
        const auto other = reel_strip::get(static_cast<ReelId>(i));
        if (!other.data || other.size == 0) continue;
        symbol[i] = other.data[ctx.stopped_position[i] % other.size];
        ready[i] = true;
    }

    const uint8_t current = static_cast<uint8_t>(ctx.reel);
    symbol[current] = strip.data[candidate % strip.size];
    ready[current] = true;

    if (!ready[0] || !ready[1] || !ready[2]) return false;

    const bool bell =
        symbol[0] == Symbol::Bell
        && symbol[1] == Symbol::Bell
        && (symbol[2] == Symbol::Bell || symbol[2] == Symbol::Red7);

    const bool replay =
        symbol[0] == Symbol::Replay
        && symbol[1] == Symbol::Replay
        && symbol[2] == Symbol::Replay;

    const bool watermelon =
        symbol[0] == Symbol::Watermelon
        && symbol[1] == Symbol::Watermelon
        && symbol[2] == Symbol::Watermelon;

    const bool penguin =
        symbol[0] == Symbol::Penguin
        && symbol[1] == Symbol::Penguin
        && symbol[2] == Symbol::Penguin;

    const bool weak_chance_substitute =
        symbol[0] == Symbol::Bell
        && symbol[1] == Symbol::Bell
        && symbol[2] == Symbol::Snow;

    const bool strong_chance_substitute =
        symbol[0] == Symbol::Watermelon
        && symbol[1] == Symbol::Watermelon
        && symbol[2] == Symbol::Snow;

    const bool red777 =
        symbol[0] == Symbol::Red7
        && symbol[1] == Symbol::Red7
        && symbol[2] == Symbol::Red7;

    const bool blue777 =
        symbol[0] == Symbol::Blue7
        && symbol[1] == Symbol::Blue7
        && symbol[2] == Symbol::Blue7;

    const bool red77bar =
        symbol[0] == Symbol::Red7
        && symbol[1] == Symbol::Red7
        && symbol[2] == Symbol::Bar;

    return bell || replay || watermelon || penguin
        || weak_chance_substitute || strong_chance_substitute
        || red777 || blue777 || red77bar;
}

bool completesVisibleBellLine(
    const stop_shared::Context& ctx,
    const reel_strip::StripView& strip,
    uint8_t candidate
) {
    uint8_t center[3]{0u,0u,0u};
    bool ready[3]{false,false,false};

    for (uint8_t i = 0; i < 3u; ++i) {
        if (!ctx.stopped[i]) continue;
        center[i] = ctx.stopped_position[i];
        ready[i] = true;
    }
    const uint8_t current = static_cast<uint8_t>(ctx.reel);
    center[current] = candidate;
    ready[current] = true;
    if (!ready[0] || !ready[1] || !ready[2]) return false;

    auto at = [&](uint8_t reel, int row) {
        const auto view = reel_strip::get(static_cast<ReelId>(reel));
        if (!view.data || view.size == 0u) return Symbol::Unknown;
        int p = static_cast<int>(center[reel]) + row;
        while (p < 0) p += view.size;
        p %= view.size;
        return view.data[p];
    };
    auto bellLine = [&](int left_row, int middle_row, int right_row) {
        return at(0u,left_row) == Symbol::Bell
            && at(1u,middle_row) == Symbol::Bell
            && at(2u,right_row) == Symbol::Bell;
    };

    return bellLine(-1,-1,-1) || bellLine(0,0,0) || bellLine(1,1,1)
        || bellLine(-1,0,1) || bellLine(1,0,-1);
}

} // namespace slotv2::stop_rules
