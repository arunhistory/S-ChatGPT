#include <iostream>
#include "assist-target/index.hpp"

int main() {
    bool ok = true;
    using slotv2::ReelId;
    using slotv2::RoleFlag;
    using slotv2::Symbol;

    ok = ok && slotv2::assist_target::accepts(RoleFlag::Bell9, ReelId::Left, Symbol::Bell);
    ok = ok && slotv2::assist_target::accepts(RoleFlag::Bell15, ReelId::Middle, Symbol::Bell);
    ok = ok && slotv2::assist_target::accepts(RoleFlag::Bell9, ReelId::Right, Symbol::Bell);
    ok = ok && slotv2::assist_target::accepts(RoleFlag::Bell9, ReelId::Right, Symbol::Red7);

    // Red7 substitution is valid only on right reel while a bell is internally active.
    ok = ok && !slotv2::assist_target::accepts(RoleFlag::Bell9, ReelId::Left, Symbol::Red7);
    ok = ok && !slotv2::assist_target::accepts(RoleFlag::Replay, ReelId::Right, Symbol::Red7);

    ok = ok && slotv2::assist_target::accepts(RoleFlag::Replay, ReelId::Left, Symbol::Replay);
    ok = ok && slotv2::assist_target::accepts(RoleFlag::Replay, ReelId::Middle, Symbol::Replay);
    ok = ok && slotv2::assist_target::accepts(RoleFlag::Replay, ReelId::Right, Symbol::Replay);

    // Entry wait uses full target assist inside the legal 0-4 slip window.
    ok = ok && slotv2::assist_target::accepts(RoleFlag::EntryAT, ReelId::Left, Symbol::Red7);
    ok = ok && slotv2::assist_target::accepts(RoleFlag::EntryAT, ReelId::Middle, Symbol::Red7);
    ok = ok && slotv2::assist_target::accepts(RoleFlag::EntryAT, ReelId::Right, Symbol::Red7);
    ok = ok && slotv2::assist_target::accepts(RoleFlag::EntryBonus, ReelId::Left, Symbol::Red7);
    ok = ok && slotv2::assist_target::accepts(RoleFlag::EntryBonus, ReelId::Middle, Symbol::Red7);
    ok = ok && slotv2::assist_target::accepts(RoleFlag::EntryBonus, ReelId::Right, Symbol::Bar);
    ok = ok && !slotv2::assist_target::accepts(RoleFlag::EntryBonus, ReelId::Right, Symbol::Red7);

    if (!ok) {
        std::cerr << "slot_v2_assist_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_assist_test: OK\n";
    return 0;
}
