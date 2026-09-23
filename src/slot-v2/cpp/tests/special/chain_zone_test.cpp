#include <iostream>
#include "special/chain_zone.hpp"

int main() {
    using namespace slotv2::chain_zone;
    bool ok=true;
    {
        State s{};
        start(s);
        for (int i=0;i<4;++i) {
            const auto g=playOne(s,false);
            ok=ok && g.active_before && !g.finished && !g.set_continued;
        }
        const auto last=playOne(s,false);
        ok=ok && last.finished && last.hits_ready==0 && !s.active;
    }
    {
        State s{};
        start(s);
        for (int set=0;set<2;++set) {
            for (int g=0;g<5;++g) {
                const auto r=playOne(s,g==0);
                if (g==4) ok=ok && r.set_continued;
                else ok=ok && !r.set_continued && !r.finished;
            }
        }
        ok=ok && s.earned_hits==1 && s.successful_continuations==2;
        for (int g=0;g<4;++g) (void)playOne(s,false);
        const auto end=playOne(s,false);
        ok=ok && end.finished && end.hits_ready==1;
    }
    {
        State s{};
        start(s);
        for (int set=0;set<8;++set) {
            for (int g=0;g<5;++g) {
                const auto r=playOne(s,g==0 || g==1);
                if (set==7 && g==4)
                    ok=ok && r.finished && r.hits_ready==4;
            }
        }
        ok=ok && !s.active && s.successful_continuations==8
            && s.earned_hits==4;
    }
    ok=ok && simultaneousUpgradePerThousand(1)==0
        && simultaneousUpgradePerThousand(2)==50
        && simultaneousUpgradePerThousand(3)==75
        && simultaneousUpgradePerThousand(4)==100;

    if (!ok) {
        std::cerr << "chain_zone_test: FAIL\n";
        return 1;
    }
    std::cout << "chain_zone_test: PASS\n";
}
