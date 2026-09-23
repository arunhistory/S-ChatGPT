#include <iostream>
#include "section/section_state.hpp"
#include "stock/stock_state.hpp"
#include "normal/normal_progress.hpp"

int main() {
    bool ok = true;

    {
        slotv2::section::State s{};
        auto a = slotv2::section::apply(s, -1000);
        ok = ok && !a.cut && s.current_diff == -1000 && s.minimum_diff == -1000;

        auto b = slotv2::section::apply(s, 2399);
        ok = ok && !b.cut && s.current_diff == 1399;

        auto c = slotv2::section::apply(s, 1);
        ok = ok && c.cut
            && c.before_reset_diff == 1400
            && c.before_reset_minimum == -1000
            && c.gain_from_minimum == 2400
            && s.current_diff == 0
            && s.minimum_diff == 0
            && s.section_count == 1;
    }

    {
        slotv2::stock::State s{};
        ok = ok && slotv2::stock::preferenceLevel(s) == 0;
        slotv2::stock::add(s, 1);
        ok = ok && slotv2::stock::preferenceLevel(s) == 1;
        slotv2::stock::add(s, 2);
        ok = ok && slotv2::stock::preferenceLevel(s) == 2;
        slotv2::stock::add(s, 2);
        ok = ok && slotv2::stock::preferenceLevel(s) == 3;
        ok = ok && slotv2::stock::consumeForSection(s) == 3 && s.count == 0;
    }

    {
        slotv2::normal_progress::State s{};
        for (int i = 0; i < 4; ++i) ok = ok && !slotv2::normal_progress::onBell9(s);
        ok = ok && slotv2::normal_progress::onBell9(s);

        ok = ok && !slotv2::normal_progress::onCZResult(s, false);
        ok = ok && !slotv2::normal_progress::onCZResult(s, false);
        ok = ok && slotv2::normal_progress::onCZResult(s, false);

        for (int i = 0; i < 5; ++i) slotv2::normal_progress::onNormalHit(s, false);
        ok = ok && slotv2::normal_progress::consumeNextATGuarantee(s);
        ok = ok && !slotv2::normal_progress::consumeNextATGuarantee(s);
    }

    if (!ok) {
        std::cerr << "slot_v2_progress_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_progress_test: OK\n";
    return 0;
}
