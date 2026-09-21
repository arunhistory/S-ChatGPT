#include <iostream>
#include "shared/rng.hpp"
#include "stock-lottery/index.hpp"
#include "bonus-upgrade/index.hpp"
#include "upper-comeback/index.hpp"
#include "special-zone/index.hpp"

int main() {
    bool ok = true;
    slotv2::Rng rng(0x55AA55AAULL);

    {
        slotv2::upper_comeback::State s{};
        slotv2::upper_comeback::start(s);
        ok = ok && s.active && s.games_left == 64;
        for (int i = 0; i < 64; ++i) slotv2::upper_comeback::consumeGame(s);
        ok = ok && !s.active && s.games_left == 0;
    }

    {
        slotv2::special_zone::State s{};
        slotv2::special_zone::start(s);
        for (int i = 0; i < 5; ++i) {
            (void)slotv2::special_zone::playOne(s, rng);
        }
        ok = ok && !s.active && s.games_left == 0;
    }

    // 経路到達性だけ確認。確率値そのものはコード上のdenominatorで固定。
    uint64_t stock=0, episode=0, comeback=0;
    for (uint64_t i = 0; i < 1000000ULL; ++i) {
        stock += slotv2::stock_lottery::onHit(rng);
        episode += slotv2::bonus_upgrade::toEpisode(rng);
        comeback += slotv2::upper_comeback::judge(rng);
    }

    ok = ok && stock > 0 && episode > 0 && comeback > 0;

    if (!ok) {
        std::cerr << "slot_v2_feature_lottery_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_feature_lottery_test: OK\n";
    return 0;
}
