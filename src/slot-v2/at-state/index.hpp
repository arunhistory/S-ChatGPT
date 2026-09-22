#pragma once
#include <stdint.h>

namespace slotv2::at_state {

enum class Tier : uint8_t {
    Lower = 0,
    Middle = 1,
    Upper = 2
};

enum class Table : uint8_t {
    Normal = 0,
    Heaven = 1,
    SuperHeaven = 2,
    Specialized = 3
};

static constexpr int kInitialSTGames = 100;
static constexpr int kLowerNetPerGame = 6;
static constexpr int kMiddleNetPerGame = 6;
static constexpr int kUpperNetPerGame = 12;
static constexpr int kUpperComebackGames = 64;

struct State {
    bool active{false};
    Tier tier{Tier::Lower};
    Table table{Table::Normal};
    int games_left{0};
    bool cold{false};
};

void start(State& state, Tier tier);
void end(State& state);
void addGames(State& state, int games);
bool consumeGame(State& state);
void setTier(State& state, Tier tier);
int netPerGame(const State& state);

} // namespace slotv2::at_state
