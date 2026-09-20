#include "slot_engine.hpp"

#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) {
    long long spins = 1000000;
    if (argc > 1) spins = std::atoll(argv[1]);
    int setting = 7;
    if (argc > 2) setting = std::atoi(argv[2]);
    if (setting < 1 || setting > 7) setting = 7;
    auto config = schatgpt::gameConfigForSetting(static_cast<schatgpt::SettingId>(setting));
    schatgpt::SlotEngine engine(0x5343484154475054ULL, config);

    long long cz = 0, bonus = 0, episode = 0, at = 0, freeze = 0;
    while (engine.state().total_games < spins) {
        const bool was_normal = !engine.state().in_at;
        auto events = engine.state().in_at ? engine.spinAT() : engine.spinNormal();

        // Browser runtime applies the normal-game reel payout after stop.
        // Mirror that here so totalDiff is a true net-difference counter.
        if (was_normal && engine.state().last_reel_payout > 0) {
            engine.applyReelPayoutJson(engine.state().last_reel_payout);
        }

        for (const auto& e : events) {
            switch (e.type) {
                case schatgpt::EventType::CZ: ++cz; break;
                case schatgpt::EventType::Bonus: ++bonus; break;
                case schatgpt::EventType::EpisodeBonus: ++episode; break;
                case schatgpt::EventType::ATStart: ++at; break;
                case schatgpt::EventType::Freeze: ++freeze; break;
                default: break;
            }
        }
    }

    const long long actual_games = engine.state().total_games;
    const long long total_in = actual_games * 3LL;
    const long long total_out = total_in + engine.state().total_diff;
    const double payout_ratio = total_in > 0
        ? static_cast<double>(total_out) / static_cast<double>(total_in)
        : 0.0;
    auto ratio = [actual_games](long long n) -> double {
        return n ? static_cast<double>(actual_games) / n : 0.0;
    };

    std::cout << "setting=" << setting << '\n'
              << "target_payout_ratio=" << (engine.config().target_payout_ratio * 100.0) << "%\n"
              << "target_games=" << spins << '\n'
              << "actual_games=" << actual_games << '\n'
              << "net_diff=" << engine.state().total_diff << '\n'
              << "payout_ratio=" << (payout_ratio * 100.0) << "%\n"
              << "CZ count=" << cz << " 1/" << ratio(cz) << '\n'
              << "bonus count=" << bonus << " 1/" << ratio(bonus) << '\n'
              << "episode count=" << episode << " 1/" << ratio(episode) << '\n'
              << "AT starts=" << at << " 1/" << ratio(at) << '\n'
              << "freeze count=" << freeze << '\n'
              << "state=" << engine.stateJson() << '\n';
}
