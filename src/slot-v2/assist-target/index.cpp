#include "index.hpp"

namespace slotv2::assist_target {

bool accepts(RoleFlag role, ReelId reel, Symbol symbol) {
    switch (role) {
        case RoleFlag::Bell9:
        case RoleFlag::Bell15:
            if (reel == ReelId::Right) {
                return symbol == Symbol::Bell || symbol == Symbol::Red7;
            }
            return symbol == Symbol::Bell;

        case RoleFlag::Replay:
            return symbol == Symbol::Replay;

        case RoleFlag::EntryAT:
            return symbol == Symbol::Red7;

        case RoleFlag::EntryBonus:
            if (reel == ReelId::Right) {
                return symbol == Symbol::Bar;
            }
            return symbol == Symbol::Red7;

        default:
            return false;
    }
}

} // namespace slotv2::assist_target
