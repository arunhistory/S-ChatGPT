#include "reel/stop/rules/rule_router.hpp"
#include "reel/stop/rules/bell_rule.hpp"
#include "reel/stop/rules/replay_rule.hpp"
#include "reel/stop/rules/cherry_rule.hpp"
#include "reel/stop/rules/watermelon_rule.hpp"
#include "reel/stop/rules/miss_rule.hpp"
#include "reel/stop/rules/one_medal_rule.hpp"
#include "reel/stop/rules/weak_chance_rule.hpp"
#include "reel/stop/rules/strong_chance_rule.hpp"
#include "reel/stop/rules/penguin_rule.hpp"
#include "reel/stop/rules/entry_rule.hpp"
#include "reel/stop/rules/fallback_rule.hpp"

namespace slotv2::stop_rules {

bool accepts(const stop_shared::Context& ctx, const reel_strip::StripView& strip, uint8_t candidate) {
    switch (ctx.role) {
        case RoleFlag::Bell9:
        case RoleFlag::Bell15:
        case RoleFlag::ThreeMedal:
            return bell::accepts(ctx, strip, candidate);

        case RoleFlag::Replay:
            return replay::accepts(ctx, strip, candidate);

        case RoleFlag::WeakCherry:
        case RoleFlag::StrongCherry:
            return cherry::accepts(ctx, strip, candidate);

        case RoleFlag::Watermelon:
            return watermelon::accepts(ctx, strip, candidate);

        case RoleFlag::WeakChance:
            return weak_chance::accepts(ctx, strip, candidate);

        case RoleFlag::StrongChance:
            return strong_chance::accepts(ctx, strip, candidate);

        case RoleFlag::PenguinChance:
            return penguin::accepts(ctx, strip, candidate);

        case RoleFlag::EntryAT:
        case RoleFlag::EntryBonus:
            return entry::accepts(ctx, strip, candidate);

        case RoleFlag::OneMedal:
            return one_medal::accepts(ctx, strip, candidate);

        case RoleFlag::Miss:
        case RoleFlag::None:
            return miss::accepts(ctx, strip, candidate);

        default:
            return fallback::accepts(ctx, strip, candidate);
    }
}

} // namespace slotv2::stop_rules
