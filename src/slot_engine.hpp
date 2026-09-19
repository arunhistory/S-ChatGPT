#pragma once

#include <array>
#include <cstdint>
#include <random>
#include <string>
#include <vector>

namespace schatgpt {

constexpr std::uint32_t NORMAL_RNG_SPACE = 134217728u;

enum class NormalMode : std::uint8_t { NormalA, NormalB, Heaven, SuperHeaven, Special };
enum class ATTier : std::uint8_t { Lower, Middle, Upper };
enum class ATTable : std::uint8_t { Normal, Heaven, SuperHeaven, Specialized };
enum class EventType : std::uint8_t {
    None, CZ, Bonus, EpisodeBonus, ATStart, ATAddGames, SpecialZone,
    UpperSpecialZone, StockGain, TierUp, TierDown, ATEnd, UpperComeback,
    Freeze, SectionCross
};

struct Event {
    EventType type{EventType::None};
    int value{0};
    std::string note{};
};

struct WeightedGames {
    int games;
    double weight;
};

struct GameConfig {
    double target_cz_rate = 1.0 / 350.0;
    double target_bonus_rate = 1.0 / 400.0;
    double target_at_rate = 1.0 / 500.0;

    double raw_cz_rate = 1.0 / 453.0;
    double raw_bonus_rate = 1.0 / 647.0;
    double raw_at_rate = 1.0 / 1275.0;
    double bonus_to_at_rate = 0.34;

    std::array<double, 4> normal_mode_weights{0.45, 0.35, 0.15, 0.05};
    double special_from_super_heaven_rate = 0.10;

    int bonus_medals = 50;
    int episode_bonus_medals = 80;
    double bonus_to_episode_rate = 0.01;
    double bonus_to_stock_rate = 0.10;
    double episode_to_middle_at_rate = 1.0 / 3.0;

    int cz_games = 10;
    double cz_base_bonus_rate = 1.0 / 100.0;
    double cz_base_at_rate = 1.0 / 1000.0;
    int cz_miss_ceiling = 3;
    int bonus_at_miss_ceiling = 5;

    std::uint32_t rare_mid_stock_den = 8192;
    std::uint32_t rare_upper_den = 32768;
    std::uint32_t freeze_den = NORMAL_RNG_SPACE;

    std::vector<WeightedGames> initial_games{
        {20, 0.20}, {30, 0.25}, {40, 0.20}, {50, 0.15},
        {75, 0.08}, {100, 0.06}, {150, 0.03}, {200, 0.025}, {300, 0.005}
    };

    std::vector<WeightedGames> add_games{
        {10, 0.30}, {20, 0.30}, {30, 0.20}, {50, 0.12}, {100, 0.06}, {200, 0.02}
    };

    std::vector<WeightedGames> special_add_games{
        {20, 0.30}, {30, 0.25}, {40, 0.20}, {50, 0.15}, {100, 0.08}, {200, 0.02}
    };

    double at_table_pattern_move_rate = 1.0 / 99.0;

    double lower_hit_rate = 1.0 / 200.0;
    double lower_fall_rate = 1.0 / 400.0;
    double lower_add_rate = 1.0 / 300.0;
    double lower_special_rate = 1.0 / 700.0;
    double lower_episode_rate = 1.0 / 1000.0;
    double lower_upper_special_rate = 1.0 / 5000.0;

    double middle_event_scale = 1.86;
    double upper_event_scale = 1.86;
    double middle_fall_rate = 1.0 / 180.0;
    double upper_fall_rate = 1.0 / 150.0;
    double middle_fall_to_lower_rate = 0.50;
    double upper_fall_to_end_rate = 0.50;

    int lower_net_per_game = 6;
    int middle_net_per_game = 6;
    int upper_net_per_game = 12;

    int upper_comeback_games = 64;
    double upper_comeback_rate = 0.20;

    double shining_star_rate = 0.10;
    int shining_star_stock_threshold = 3;

    int special_zone_games = 5;
    double special_zone_hit_rate = 0.50;
    double special_result_add_rate = 0.95;
    double special_result_bonus_rate = 0.05;

    struct ChainPreset { int chains; double weight; };
    std::vector<ChainPreset> upper_special_chains{
        {1,0.12},{2,0.10},{3,0.08},{5,0.08},{8,0.10},{12,0.12},
        {16,0.12},{24,0.11},{32,0.08},{50,0.05},{100,0.04}
    };

    std::array<int,4> stock_thresholds{0,1,3,5};
};

struct MachineState {
    int setting = 6;
    NormalMode normal_mode = NormalMode::NormalA;
    int normal_pattern = 0;
    int normal_games = 0;
    int normal_ceiling = 1500;
    bool special_window_checked = false;
    int cz_misses = 0;
    int bonus_at_misses = 0;

    bool in_at = false;
    ATTier at_tier = ATTier::Lower;
    ATTable at_table = ATTable::Normal;
    int at_pattern = 0;
    int at_games_left = 0;
    int stocks = 0;

    // 6.5号機型の有利区間差枚管理。
    // section_diff は「有利区間開始時=0」を基準にした現在差枚。
    // section_min_diff はデバッグ/履歴用で、2400枚切断判定の基準には使わない。
    long long section_diff = 0;
    long long section_min_diff = 0;
    long long total_diff = 0;
    long long section_count = 0;
    long long total_games = 0;
};

class SlotEngine {
public:
    explicit SlotEngine(std::uint64_t seed = 0x5343484154475054ULL, GameConfig config = {});

    void reset(std::uint64_t seed);
    std::vector<Event> spinNormal();
    std::vector<Event> spinAT();
    std::vector<Event> playCZ();
    std::vector<Event> playBonus();
    std::vector<Event> playSpecialZone(bool upper);

    const MachineState& state() const { return state_; }
    const GameConfig& config() const { return config_; }

    std::string stateJson() const;
    std::string spinNormalJson();
    std::string spinATJson();
    std::string applyReelPayoutJson(int medals);

private:
    GameConfig config_;
    MachineState state_;
    std::mt19937_64 rng_;

    double uniform01();
    bool chance(double p);
    int weightedGames(const std::vector<WeightedGames>& table);
    int weightedUpperChains();

    void rerollNormalModeAndPattern();
    void rerollATTableAndPattern();
    int chooseNormalCeiling(NormalMode mode, int pattern);
    void startAT(ATTier tier, bool withStock, std::vector<Event>& out, const char* reason);
    void endAT(std::vector<Event>& out);
    void applySectionDelta(long long medals, std::vector<Event>& out);
    int stockPreferenceLevel() const;

    std::vector<Event> resolveNormalCeiling();
    std::vector<Event> resolveATEvent();
    std::vector<Event> resolveUpperComeback();

    static const char* normalModeName(NormalMode v);
    static const char* atTierName(ATTier v);
    static const char* atTableName(ATTable v);
    static const char* eventName(EventType v);
    static std::string eventsJson(const std::vector<Event>& events, const MachineState& state);
};

} // namespace schatgpt
