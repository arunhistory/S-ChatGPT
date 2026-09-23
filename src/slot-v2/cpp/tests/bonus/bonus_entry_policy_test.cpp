#include <iostream>
#include "bonus-entry-policy/index.hpp"

int main() {
    bool ok = true;

    const auto a = slotv2::bonus_entry_policy::forMode(
        slotv2::normal_mode::Mode::NormalA
    );
    const auto b = slotv2::bonus_entry_policy::forMode(
        slotv2::normal_mode::Mode::NormalB
    );
    const auto h = slotv2::bonus_entry_policy::forMode(
        slotv2::normal_mode::Mode::Heaven
    );
    const auto sh = slotv2::bonus_entry_policy::forMode(
        slotv2::normal_mode::Mode::SuperHeaven
    );
    const auto sp = slotv2::bonus_entry_policy::forMode(
        slotv2::normal_mode::Mode::Special
    );

    ok = ok && a.kind == slotv2::bonus_state::Kind::Regular;
    ok = ok && b.kind == slotv2::bonus_state::Kind::Regular;
    ok = ok && h.kind == slotv2::bonus_state::Kind::Episode;
    ok = ok && sh.kind == slotv2::bonus_state::Kind::Episode;
    ok = ok && sp.kind == slotv2::bonus_state::Kind::Regular;

    if (!ok) {
        std::cerr << "slot_v2_bonus_entry_policy_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_bonus_entry_policy_test: OK\n";
    return 0;
}
