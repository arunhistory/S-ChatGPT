#include <iostream>
#include "at/at_state.hpp"
#include "bonus/bonus_state.hpp"
#include "cz/cz_state.hpp"

int main() {
    bool ok = true;

    {
        slotv2::at_state::State s{};
        slotv2::at_state::start(s, slotv2::at_state::Tier::Lower);
        ok = ok && s.active && s.games_left == 100;
        ok = ok && slotv2::at_state::netPerGame(s) == 6;

        slotv2::at_state::setTier(s, slotv2::at_state::Tier::Middle);
        ok = ok && slotv2::at_state::netPerGame(s) == 6;

        slotv2::at_state::setTier(s, slotv2::at_state::Tier::Upper);
        ok = ok && slotv2::at_state::netPerGame(s) == 12;

        slotv2::at_state::addGames(s, 50);
        ok = ok && s.games_left == 150;
    }

    {
        slotv2::bonus_state::State s{};
        slotv2::bonus_state::start(s, slotv2::bonus_state::Kind::Regular);
        ok = ok && s.medals_left == 50;
        slotv2::bonus_state::applyNetGain(s, 50);
        ok = ok && slotv2::bonus_state::complete(s);

        slotv2::bonus_state::start(s, slotv2::bonus_state::Kind::Episode);
        ok = ok && s.medals_left == 80;
    }

    {
        slotv2::cz_state::State s{};
        slotv2::cz_state::start(s);
        ok = ok && s.active && s.games_left == 10;
        for (int i = 0; i < 10; ++i) slotv2::cz_state::consumeGame(s);
        ok = ok && !s.active && s.games_left == 0;
    }

    if (!ok) {
        std::cerr << "slot_v2_state_modules_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_state_modules_test: OK\n";
    return 0;
}
