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

    // 🟥7代用は右ベル成立時だけ。
    ok = ok && !slotv2::assist_target::accepts(RoleFlag::Bell9, ReelId::Left, Symbol::Red7);
    ok = ok && !slotv2::assist_target::accepts(RoleFlag::Replay, ReelId::Right, Symbol::Red7);

    ok = ok && slotv2::assist_target::accepts(RoleFlag::Replay, ReelId::Left, Symbol::Replay);
    ok = ok && slotv2::assist_target::accepts(RoleFlag::Replay, ReelId::Middle, Symbol::Replay);
    ok = ok && slotv2::assist_target::accepts(RoleFlag::Replay, ReelId::Right, Symbol::Replay);

    if (!ok) {
        std::cerr << "slot_v2_assist_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_assist_test: OK\n";
    return 0;
}
