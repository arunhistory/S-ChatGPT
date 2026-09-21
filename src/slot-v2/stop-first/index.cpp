#include "index.hpp"
#include "../stop-candidate/index.hpp"
#include "../reel-strip/index.hpp"

namespace slotv2::stop_first {
namespace {

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

bool legalForRole(const stop_shared::Context& ctx, const reel_strip::StripView& strip, uint8_t candidate) {
    // この台の独自ルール:
    // チェリーフラグ以外では左の可視3コマに🍒を一切出さない。
    if (ctx.reel == ReelId::Left
        && ctx.role != RoleFlag::WeakCherry
        && ctx.role != RoleFlag::StrongCherry
        && visibleHas(strip, candidate, Symbol::Cherry)) {
        return false;
    }

    // 中段1ライン。目押し不要役はその図柄自身を中段へ引き込む。
    if (ctx.role == RoleFlag::Bell9 || ctx.role == RoleFlag::Bell15) {
        return centerIs(strip, candidate, Symbol::Bell);
    }
    if (ctx.role == RoleFlag::Replay) {
        return centerIs(strip, candidate, Symbol::Replay);
    }

    // 左リールの目押し役。
    if (ctx.reel == ReelId::Left && ctx.role == RoleFlag::StrongCherry) {
        return centerIs(strip, candidate, Symbol::Cherry);
    }
    if (ctx.reel == ReelId::Left && ctx.role == RoleFlag::Watermelon) {
        return centerIs(strip, candidate, Symbol::Watermelon);
    }

    // 弱チェは左下段🍒を狙う。center+1 が下段。
    if (ctx.reel == ReelId::Left && ctx.role == RoleFlag::WeakCherry) {
        int p = (static_cast<int>(candidate) + 1) % strip.size;
        return strip.data[p] == Symbol::Cherry;
    }

    // 未確定役は「成立していない🍒を見せない」だけ守り、押した位置優先。
    return true;
}

}

stop_shared::Result resolve(const stop_shared::Context& ctx) {
    const auto strip = reel_strip::get(ctx.reel);
    if (!strip.data || strip.size == 0) {
        return {stop_shared::ResolveStatus::StripUndefined, ctx.pressed_position, 0};
    }

    const auto candidates = stop_candidate::build(ctx.pressed_position);
    for (uint8_t i = 0; i < candidates.count; ++i) {
        const uint8_t candidate = candidates.position[i];
        if (legalForRole(ctx, strip, candidate)) {
            return {stop_shared::ResolveStatus::Ok, candidate, i};
        }
    }

    // 目押し役は届かなければ取りこぼす。
    // ただしチェリー非成立時に🍒が見える候補は採用しない。
    return {stop_shared::ResolveStatus::NoLegalCandidate, ctx.pressed_position, 0};
}

} // namespace slotv2::stop_first
