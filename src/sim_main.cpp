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
    for (long long i = 0; i < spins; ++i) {
        auto events = engine.state().in_at ? engine.spinAT() : engine.spinNormal();
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

    auto ratio = [spins](long long n) -> double { return n ? static_cast<double>(spins) / n : 0.0; };
    std::cout << "setting=" << setting << '\n'
              << "target_payout_ratio=" << (engine.config().target_payout_ratio * 100.0) << "%\n"
              << "spins=" << spins << '\n'
              << "CZ count=" << cz << " 1/" << ratio(cz) << '\n'
              << "bonus count=" << bonus << " 1/" << ratio(bonus) << '\n'
              << "episode count=" << episode << " 1/" << ratio(episode) << '\n'
              << "AT starts=" << at << " 1/" << ratio(at) << '\n'
              << "freeze count=" << freeze << '\n'
              << "state=" << engine.stateJson() << '\n';
}
