#include "slot_engine.hpp"

#include <algorithm>
#include <sstream>

namespace schatgpt {
namespace {

template <typename T>
T clampv(T v, T lo, T hi) { return std::min(hi, std::max(lo, v)); }

std::string escapeJson(const std::string& s) {
    std::ostringstream o;
    for (char c : s) {
        switch (c) {
            case '"': o << "\\\""; break;
            case '\\': o << "\\\\"; break;
            case '\n': o << "\\n"; break;
            case '\r': o << "\\r"; break;
            case '\t': o << "\\t"; break;
            default: o << c;
        }
    }
    return o.str();
}

} // namespace

GameConfig gameConfigForSetting(SettingId setting) {
    GameConfig c;
    c.setting = static_cast<int>(setting);

    // 長期目標機械割。シミュレータはこの値との差を較正対象にする。
    switch (setting) {
        case SettingId::S1: c.target_payout_ratio = 0.85; break;
        case SettingId::S2: c.target_payout_ratio = 0.96; break;
        case SettingId::S3: c.target_payout_ratio = 0.99; break;
        case SettingId::S4: c.target_payout_ratio = 1.03; break;
        case SettingId::S5: c.target_payout_ratio = 1.08; break;
        case SettingId::S6: c.target_payout_ratio = 1.14; break;
        case SettingId::EX: c.target_payout_ratio = 1.50; break;
    }

    // 設定別の性能プロファイル。ゲームルールは共通で、確率ノブだけを変更する。
    const auto useReducedUpper = [&]() {
        c.upper_special_chains = {
            {1,0.20},{2,0.18},{3,0.16},{4,0.14},{5,0.11},
            {6,0.08},{8,0.06},{12,0.04},{16,0.03}
        };
    };

    switch (setting) {
        case SettingId::S1:
            // L相当: ボーナスは非常に軽いがAT期待を極端に抑え、長期約85%を狙う。
            c.raw_at_rate = 1.0 / 5000.0;
            c.raw_bonus_rate = 1.0 / 136.94;
            c.raw_cz_rate = 1.0 / 700.0;
            c.bonus_to_stock_rate = 0.01102;
            c.upper_comeback_rate = 0.02255;
            c.lower_hit_rate = 1.0 / 873.79;
            c.lower_fall_rate = 1.0 / 172.51;
            c.lower_add_rate = 1.0 / 1046.62;
            c.lower_special_rate = 1.0 / 2494.0;
            c.lower_upper_special_rate = 1.0 / 16921.27;
            c.middle_event_scale = c.upper_event_scale = 1.1204;
            c.bonus_to_at_rate = 0.0851;
            c.bonus_at_miss_ceiling = 999;
            useReducedUpper();
            return c;

        case SettingId::S2:
            c.raw_at_rate = 1.0 / 1465.08;
            c.raw_bonus_rate = 1.0 / 697.79;
            c.raw_cz_rate = 1.0 / 478.97;
            c.bonus_to_stock_rate = 0.08541;
            c.upper_comeback_rate = 0.16082;
            c.lower_hit_rate = 1.0 / 221.63;
            c.lower_fall_rate = 1.0 / 362.96;
            c.lower_add_rate = 1.0 / 348.13;
            c.lower_special_rate = 1.0 / 810.64;
            c.lower_upper_special_rate = 1.0 / 5860.31;
            c.middle_event_scale = c.upper_event_scale = 1.72869;
            c.bonus_to_at_rate = 0.28164;
            useReducedUpper();
            return c;

        case SettingId::S3:
            c.raw_at_rate = 1.0 / 1393.28;
            c.raw_bonus_rate = 1.0 / 675.52;
            c.raw_cz_rate = 1.0 / 471.33;
            c.bonus_to_stock_rate = 0.09285;
            c.upper_comeback_rate = 0.17570;
            c.lower_hit_rate = 1.0 / 211.59;
            c.lower_fall_rate = 1.0 / 373.40;
            c.lower_add_rate = 1.0 / 327.10;
            c.lower_special_rate = 1.0 / 764.66;
            c.lower_upper_special_rate = 1.0 / 5464.91;
            c.middle_event_scale = c.upper_event_scale = 1.7856;
            c.bonus_to_at_rate = 0.3014;
            useReducedUpper();
            return c;

        case SettingId::S4:
            c.raw_at_rate = 1.0 / 1331.24;
            c.raw_bonus_rate = 1.0 / 652.50;
            c.raw_cz_rate = 1.0 / 461.25;
            c.bonus_to_stock_rate = 0.09949;
            c.upper_comeback_rate = 0.18874;
            c.lower_hit_rate = 1.0 / 201.00;
            c.lower_fall_rate = 1.0 / 388.72;
            c.lower_add_rate = 1.0 / 302.47;
            c.lower_special_rate = 1.0 / 704.95;
            c.lower_upper_special_rate = 1.0 / 5037.12;
            c.middle_event_scale = c.upper_event_scale = 1.84988;
            c.bonus_to_at_rate = 0.31747;
            useReducedUpper();
            return c;

        case SettingId::S5:
            c.raw_at_rate = 1.0 / 1259.57;
            c.raw_bonus_rate = 1.0 / 620.62;
            c.raw_cz_rate = 1.0 / 447.82;
            c.bonus_to_stock_rate = 0.10974;
            c.upper_comeback_rate = 0.20460;
            c.lower_hit_rate = 1.0 / 190.24;
            c.lower_fall_rate = 1.0 / 404.15;
            c.lower_add_rate = 1.0 / 270.57;
            c.lower_special_rate = 1.0 / 641.17;
            c.lower_upper_special_rate = 1.0 / 4433.65;
            c.middle_event_scale = c.upper_event_scale = 1.97736;
            c.bonus_to_at_rate = 0.33217;
            useReducedUpper();
            return c;

        case SettingId::S6:
            c.target_cz_rate = 1.0 / 350.0;
            c.target_bonus_rate = 1.0 / 400.0;
            c.target_at_rate = 1.0 / 500.0;
            c.raw_cz_rate = 1.0 / 440.0;
            c.raw_bonus_rate = 1.0 / 600.0;
            c.raw_at_rate = 1.0 / 1200.0;
            c.bonus_to_stock_rate = 0.12;
            c.upper_comeback_rate = 0.22;
            c.lower_hit_rate = 1.0 / 180.0;
            c.lower_fall_rate = 1.0 / 420.0;
            c.lower_add_rate = 1.0 / 240.0;
            c.lower_special_rate = 1.0 / 600.0;
            c.lower_upper_special_rate = 1.0 / 4000.0;
            c.middle_event_scale = c.upper_event_scale = 2.10;
            useReducedUpper();
            return c;

        case SettingId::EX:
            c.raw_cz_rate = 1.0 / 414.0;
            c.raw_bonus_rate = 1.0 / 548.0;
            c.raw_at_rate = 1.0 / 1065.0;
            c.bonus_to_stock_rate = 0.145;
            c.upper_comeback_rate = 0.255;
            c.lower_hit_rate = 1.0 / 164.0;
            c.lower_fall_rate = 1.0 / 452.0;
            c.lower_add_rate = 1.0 / 203.0;
            c.lower_special_rate = 1.0 / 502.0;
            c.lower_upper_special_rate = 1.0 / 3075.0;
            c.middle_event_scale = c.upper_event_scale = 2.37;
            return c;
    }

    return c;
}

SlotEngine::SlotEngine(std::uint64_t seed, GameConfig config)
    : config_(std::move(config)), rng_(seed) {
    state_.setting = config_.setting;
    rerollNormalModeAndPattern();
}

void SlotEngine::reset(std::uint64_t seed) {
    rng_.seed(seed);
    state_ = {};
    state_.setting = config_.setting;
    rerollNormalModeAndPattern();
}

double SlotEngine::uniform01() {
    return std::generate_canonical<double, 53>(rng_);
}

bool SlotEngine::chance(double p) {
    if (p <= 0.0) return false;
    if (p >= 1.0) return true;
    return uniform01() < p;
}

int SlotEngine::weightedGames(const std::vector<WeightedGames>& table) {
    const double r = uniform01();
    double acc = 0.0;
    for (const auto& e : table) {
        acc += e.weight;
        if (r < acc) return e.games;
    }
    return table.empty() ? 0 : table.back().games;
}

int SlotEngine::weightedUpperChains() {
    const double r = uniform01();
    double acc = 0.0;
    for (const auto& e : config_.upper_special_chains) {
        acc += e.weight;
        if (r < acc) return e.chains;
    }
    return config_.upper_special_chains.empty() ? 1 : config_.upper_special_chains.back().chains;
}

ReelRole SlotEngine::drawNormalReelRole() {
    // 2^27マスから1回だけ引く排他的な成立役抽選。
    // 残り11,744,051マスは今後のチェリー/スイカ/🐧/チャンス目/ハズレ用。
    std::uint32_t draw = static_cast<std::uint32_t>(rng_() & (NORMAL_RNG_SPACE - 1u));

    if (draw < ROLE_ONE_MEDAL_COUNT) return ReelRole::OneMedal;
    draw -= ROLE_ONE_MEDAL_COUNT;
    if (draw < ROLE_BELL9_COUNT) return ReelRole::Bell9;
    draw -= ROLE_BELL9_COUNT;
    if (draw < ROLE_BELL15_COUNT) return ReelRole::Bell15;
    draw -= ROLE_BELL15_COUNT;
    if (draw < ROLE_REPLAY_COUNT) return ReelRole::Replay;
    return ReelRole::Miss;
}

void SlotEngine::rerollNormalModeAndPattern() {
    const double r = uniform01();
    double acc = 0.0;
    int pick = 0;
    for (int i = 0; i < 4; ++i) {
        acc += config_.normal_mode_weights[i];
        if (r < acc) { pick = i; break; }
    }
    state_.normal_mode = static_cast<NormalMode>(pick);
    state_.normal_pattern = static_cast<int>(rng_() % 10ULL);
    state_.normal_actual_games = 0;
    state_.normal_display_games = 0;
    state_.special_window_checked = false;
    state_.normal_ceiling = chooseNormalCeiling(state_.normal_mode, state_.normal_pattern);
}

int SlotEngine::chooseNormalCeiling(NormalMode mode, int pattern) {
    static constexpr std::array<int,10> A{500,700,750,900,1000,1100,1250,1350,1450,1500};
    static constexpr std::array<int,10> B{250,300,400,500,600,700,750,800,1000,1250};
    static constexpr std::array<int,10> H{100,200,250,300,400,500,600,700,750,750};
    static constexpr std::array<int,10> SH{50,50,100,100,200,200,250,250,300,300};
    pattern = clampv(pattern, 0, 9);
    switch (mode) {
        case NormalMode::NormalA: return A[pattern];
        case NormalMode::NormalB: return B[pattern];
        case NormalMode::Heaven: return H[pattern];
        case NormalMode::SuperHeaven: return SH[pattern];
        case NormalMode::Special: return chance(0.95) ? 777 : 1500;
    }
    return 1500;
}

void SlotEngine::advanceNormalDisplayGames(int games) {
    // 通常1G消化時は+1。今後の高確率「短縮」当選時はここへ追加Gを渡す。
    // 実回転数(normal_actual_games)は一切変更しない。
    if (games > 0) state_.normal_display_games += games;
}

bool SlotEngine::canEnterHighProbability() const {
    // 「50Gまでは入らない」ため、解禁は実回転数51G目から。
    return state_.normal_actual_games > config_.high_probability_block_through_actual_games;
}

void SlotEngine::rerollATTableAndPattern() {
    state_.at_table = static_cast<ATTable>(rng_() % 4ULL);
    state_.at_pattern = static_cast<int>(rng_() % 5ULL);
}

void SlotEngine::startAT(ATTier tier, bool withStock, std::vector<Event>& out, const char* reason) {
    state_.in_at = true;
    state_.at_tier = tier;
    state_.at_games_left = weightedGames(config_.initial_games);
    if (withStock) ++state_.stocks;
    rerollATTableAndPattern();

    std::ostringstream note;
    note << reason << "; initial=" << state_.at_games_left << "G";
    if (tier == ATTier::Upper && state_.stocks >= config_.shining_star_stock_threshold && chance(config_.shining_star_rate)) {
        note << "; Shining Star indication";
    }
    out.push_back({EventType::ATStart, state_.at_games_left, note.str()});
}

void SlotEngine::applySectionDelta(long long medals, std::vector<Event>& out) {
    // 6.5号機型: 有利区間開始時を差枚0として管理する。
    // 「区間内の最低差枚から+2400」ではなく、現在の区間差枚が+2400に到達したら区間を切る。
    state_.section_diff += medals;
    state_.total_diff += medals;
    if (state_.section_diff < state_.section_min_diff) {
        state_.section_min_diff = state_.section_diff;
    }

    if (state_.section_diff >= 2400) {
        const int pref = stockPreferenceLevel();
        const long long ended_diff = state_.section_diff;
        const long long ended_min = state_.section_min_diff;

        // ストックは次区間へ個数のまま持ち越さず、0/1/3/5閾値の優遇レベルへ変換。
        state_.stocks = 0;
        ++state_.section_count;

        // 次の有利区間は新しい差枚0から開始。
        state_.section_diff = 0;
        state_.section_min_diff = 0;

        std::ostringstream note;
        note << "6.5 section cut at +" << ended_diff
             << " from section start; section minimum=" << ended_min
             << "; one next-section preference lottery level=" << pref;
        out.push_back({EventType::SectionCross, pref, note.str()});

        // 現在は次区間開始時の内部ATテーブル/パターン再抽選のみ実装。
        // 0/1/3/5優遇レベル別の最終ルーレット重みは未決定のため勝手に固定しない。
        rerollATTableAndPattern();
    }
}

int SlotEngine::stockPreferenceLevel() const {
    if (state_.stocks >= 5) return 3;
    if (state_.stocks >= 3) return 2;
    if (state_.stocks >= 1) return 1;
    return 0;
}

std::vector<Event> SlotEngine::playCZ() {
    std::vector<Event> out;
    bool resolved = false;
    for (int g = 1; g <= config_.cz_games && !resolved; ++g) {
        ++state_.total_games;
        // CZも実ゲームなので3枚BETを差枚へ反映。
        applySectionDelta(-3, out);

        if (chance(config_.cz_base_at_rate)) {
            startAT(ATTier::Lower, false, out, "CZ direct AT");
            state_.cz_misses = 0;
            resolved = true;
            break;
        }
        if (chance(config_.cz_base_bonus_rate)) {
            out.push_back({EventType::Bonus, config_.bonus_medals, "CZ bonus hit"});
            auto b = playBonus();
            out.insert(out.end(), b.begin(), b.end());
            state_.cz_misses = 0;
            resolved = true;
            break;
        }
    }
    if (!resolved) {
        ++state_.cz_misses;
        if (state_.cz_misses >= config_.cz_miss_ceiling) {
            state_.cz_misses = 0;
            out.push_back({EventType::Bonus, config_.bonus_medals, "CZ 3-miss ceiling -> bonus"});
            auto b = playBonus();
            out.insert(out.end(), b.begin(), b.end());
        }
    }
    return out;
}

std::vector<Event> SlotEngine::playBonus() {
    std::vector<Event> out;
    const bool wasInAT = state_.in_at;
    applySectionDelta(config_.bonus_medals, out);

    if (chance(config_.bonus_to_stock_rate)) {
        ++state_.stocks;
        out.push_back({EventType::StockGain, state_.stocks,
                       state_.setting == static_cast<int>(SettingId::S6)
                           ? "bonus 8% stock lottery (S6)"
                           : "bonus 10% stock lottery"});
    }

    if (chance(config_.bonus_to_episode_rate)) {
        applySectionDelta(config_.episode_bonus_medals, out);
        out.push_back({EventType::EpisodeBonus, config_.episode_bonus_medals, "1% post-bonus episode promotion"});
        if (chance(config_.episode_to_middle_at_rate)) {
            if (wasInAT) {
                if (state_.at_tier == ATTier::Lower) {
                    state_.at_tier = ATTier::Middle;
                    out.push_back({EventType::TierUp, 0, "episode 1/3 -> middle AT"});
                }
            } else {
                startAT(ATTier::Middle, false, out, "episode 1/3 middle AT");
                state_.bonus_at_misses = 0;
                rerollNormalModeAndPattern();
                return out;
            }
        }
    }

    if (wasInAT) return out;

    if (chance(config_.bonus_to_at_rate) || state_.bonus_at_misses >= config_.bonus_at_miss_ceiling - 1) {
        state_.bonus_at_misses = 0;
        startAT(ATTier::Lower, false, out, "bonus performance cleared -> AT");
    } else {
        ++state_.bonus_at_misses;
    }
    rerollNormalModeAndPattern();
    return out;
}

std::vector<Event> SlotEngine::resolveNormalCeiling() {
    std::vector<Event> out;
    if (state_.normal_mode == NormalMode::Special) {
        if (state_.normal_ceiling == 777) {
            const int result = static_cast<int>(rng_() % 3ULL);
            if (result == 0) startAT(ATTier::Lower, false, out, "special 777 ceiling: AT");
            else if (result == 1) startAT(ATTier::Middle, true, out, "special 777 ceiling: 1/8192-equivalent reward");
            else startAT(ATTier::Upper, false, out, "special 777 ceiling: 1/32768-equivalent reward");
        } else {
            out.push_back({EventType::Freeze, 0, "special 1500 ceiling: freeze-favored"});
            startAT(ATTier::Upper, true, out, "special 1500 ceiling freeze reward");
        }
        rerollNormalModeAndPattern();
        return out;
    }

    const double r = uniform01();
    if (r < 0.70) {
        out.push_back({EventType::CZ, 0, "normal ceiling -> CZ"});
        auto c = playCZ(); out.insert(out.end(), c.begin(), c.end());
    } else if (r < 0.95) {
        out.push_back({EventType::Bonus, config_.bonus_medals, "normal ceiling -> bonus"});
        auto b = playBonus(); out.insert(out.end(), b.begin(), b.end());
    } else {
        startAT(ATTier::Lower, false, out, "normal ceiling -> AT");
        rerollNormalModeAndPattern();
    }
    return out;
}

std::vector<Event> SlotEngine::spinNormal() {
    std::vector<Event> out;
    if (state_.in_at) return out;

    ++state_.total_games;
    ++state_.normal_actual_games;
    advanceNormalDisplayGames(1);

    // 通常時の成立役は2^27の整数マスから排他的に抽選。
    state_.last_reel_role = drawNormalReelRole();
    switch (state_.last_reel_role) {
        case ReelRole::OneMedal: state_.last_reel_payout = 1; break;
        case ReelRole::Bell9: state_.last_reel_payout = 9; break;
        case ReelRole::Bell15: state_.last_reel_payout = 15; break;
        case ReelRole::Replay: state_.last_reel_payout = 3; break; // 3枚BETを相殺
        case ReelRole::Miss: state_.last_reel_payout = 0; break;
    }

    // 通常遊技は3枚BET。成立役の戻しはリール停止後にWASM APIから加算する。
    applySectionDelta(-3, out);

    if (state_.normal_mode == NormalMode::SuperHeaven && !state_.special_window_checked && state_.normal_actual_games >= 20) {
        state_.special_window_checked = true;
        if (chance(config_.special_from_super_heaven_rate)) {
            state_.normal_mode = NormalMode::Special;
            state_.normal_pattern = static_cast<int>(rng_() % 10ULL);
            state_.normal_ceiling = chooseNormalCeiling(NormalMode::Special, state_.normal_pattern);
        }
    }

    const std::uint32_t draw = static_cast<std::uint32_t>(rng_() & (NORMAL_RNG_SPACE - 1u));
    if (draw == 0u) {
        out.push_back({EventType::Freeze, 0, "1/134217728 freeze"});
        startAT(ATTier::Upper, true, out, "freeze -> upper AT + stock");
        rerollNormalModeAndPattern();
        return out;
    }
    if (draw < NORMAL_RNG_SPACE / config_.rare_upper_den + 1u) {
        startAT(ATTier::Upper, false, out, "1/32768 upper AT direct");
        rerollNormalModeAndPattern();
        return out;
    }
    if (draw < NORMAL_RNG_SPACE / config_.rare_mid_stock_den + NORMAL_RNG_SPACE / config_.rare_upper_den + 1u) {
        startAT(ATTier::Middle, true, out, "1/8192 middle AT + stock");
        rerollNormalModeAndPattern();
        return out;
    }

    if (chance(config_.raw_at_rate)) {
        startAT(ATTier::Lower, false, out, "raw AT route");
        rerollNormalModeAndPattern();
        return out;
    }
    if (chance(config_.raw_bonus_rate)) {
        out.push_back({EventType::Bonus, config_.bonus_medals, "raw bonus"});
        auto b = playBonus(); out.insert(out.end(), b.begin(), b.end());
        return out;
    }
    if (chance(config_.raw_cz_rate)) {
        out.push_back({EventType::CZ, 0, "raw CZ"});
        auto c = playCZ(); out.insert(out.end(), c.begin(), c.end());
        return out;
    }

    // テーブル天井は表示回転数で管理。短縮で表示Gが天井へ到達しても発動する。
    if (state_.normal_display_games >= state_.normal_ceiling) return resolveNormalCeiling();
    return out;
}

std::vector<Event> SlotEngine::playSpecialZone(bool upper) {
    std::vector<Event> out;
    if (!upper) {
        for (int g = 0; g < config_.special_zone_games; ++g) {
            if (!chance(config_.special_zone_hit_rate)) continue;
            if (chance(config_.special_result_add_rate)) {
                const int add = weightedGames(config_.special_add_games);
                state_.at_games_left += add;
                out.push_back({EventType::ATAddGames, add, "special zone chained add"});
            } else {
                out.push_back({EventType::Bonus, config_.bonus_medals, "special zone bonus"});
                auto b = playBonus(); out.insert(out.end(), b.begin(), b.end());
            }
        }
        return out;
    }

    const int chains = weightedUpperChains();
    for (int c = 0; c < chains; ++c) {
        for (int g = 0; g < 3; ++g) {
            const bool hasWin = chance(0.5);
            const int op = hasWin ? static_cast<int>(rng_() % 4ULL) : -1;
            (void)op;
        }
        if (chance(config_.special_result_add_rate)) {
            const int add = weightedGames(config_.special_add_games);
            state_.at_games_left += add;
            out.push_back({EventType::ATAddGames, add, "upper-special preset add"});
        } else {
            out.push_back({EventType::Bonus, config_.bonus_medals, "upper-special preset bonus"});
            auto b = playBonus(); out.insert(out.end(), b.begin(), b.end());
        }
    }
    return out;
}

std::vector<Event> SlotEngine::resolveATEvent() {
    std::vector<Event> out;

    double scale = 1.0;
    if (state_.at_tier == ATTier::Middle) scale = config_.middle_event_scale;
    if (state_.at_tier == ATTier::Upper) scale = config_.upper_event_scale;

    double hit = config_.lower_hit_rate * scale;
    double add = config_.lower_add_rate * scale;
    double special = config_.lower_special_rate * scale;
    double episode = config_.lower_episode_rate * scale;
    double upperSpecial = config_.lower_upper_special_rate * scale;
    double fall = config_.lower_fall_rate;
    if (state_.at_tier == ATTier::Middle) fall = config_.middle_fall_rate;
    if (state_.at_tier == ATTier::Upper) fall = config_.upper_fall_rate;

    switch (state_.at_table) {
        case ATTable::Normal: break;
        case ATTable::Heaven:
            hit *= 1.9;
            episode *= 1.4;
            break;
        case ATTable::SuperHeaven:
            add *= 2.4;
            episode *= 1.25;
            break;
        case ATTable::Specialized:
            hit *= 0.55;
            add *= 0.55;
            special *= 2.8;
            upperSpecial *= 2.5;
            break;
    }

    static constexpr double patternFactor[5]{0.82, 0.92, 1.00, 1.10, 1.20};
    const double pf = patternFactor[clampv(state_.at_pattern, 0, 4)];
    hit *= pf; add *= pf; special *= pf;

    const double total = hit + fall + add + special
        + ((state_.at_table == ATTable::Heaven || state_.at_table == ATTable::SuperHeaven) ? episode : 0.0)
        + ((state_.at_table == ATTable::Specialized) ? upperSpecial : 0.0);
    const double r = uniform01();
    if (r >= total) return out;
    double x = r;

    if ((x -= hit) < 0.0) {
        out.push_back({EventType::Bonus, config_.bonus_medals, "AT normal hit -> 50 medal bonus"});
        auto b = playBonus(); out.insert(out.end(), b.begin(), b.end());
        return out;
    }
    if ((x -= fall) < 0.0) {
        if (state_.at_tier == ATTier::Middle) {
            if (chance(config_.middle_fall_to_lower_rate)) {
                state_.at_tier = ATTier::Lower;
                out.push_back({EventType::TierDown, 0, "middle fall -> lower AT"});
            }
        } else if (state_.at_tier == ATTier::Upper) {
            if (chance(config_.upper_fall_to_end_rate)) endAT(out);
        }
        return out;
    }
    if ((x -= add) < 0.0) {
        const int g = weightedGames(config_.add_games);
        state_.at_games_left += g;
        out.push_back({EventType::ATAddGames, g, "one-shot add"});
        return out;
    }
    if ((x -= special) < 0.0) {
        out.push_back({EventType::SpecialZone, 0, "special zone"});
        auto z = playSpecialZone(false); out.insert(out.end(), z.begin(), z.end());
        return out;
    }

    const bool episodeAllowed = state_.at_table == ATTable::Heaven || state_.at_table == ATTable::SuperHeaven;
    if (episodeAllowed) {
        if ((x -= episode) < 0.0) {
            applySectionDelta(config_.episode_bonus_medals, out);
            out.push_back({EventType::EpisodeBonus, config_.episode_bonus_medals, "direct episode in heaven/super-heaven"});
            if (state_.at_tier == ATTier::Lower && chance(config_.episode_to_middle_at_rate)) {
                state_.at_tier = ATTier::Middle;
                out.push_back({EventType::TierUp, 0, "episode 1/3 -> middle AT"});
            }
            return out;
        }
    }

    if (state_.at_table == ATTable::Specialized && (x -= upperSpecial) < 0.0) {
        out.push_back({EventType::UpperSpecialZone, 0, "upper special zone"});
        auto z = playSpecialZone(true); out.insert(out.end(), z.begin(), z.end());
        return out;
    }
    return out;
}

std::vector<Event> SlotEngine::resolveUpperComeback() {
    std::vector<Event> out;
    if (chance(config_.upper_comeback_rate)) {
        state_.in_at = true;
        state_.at_tier = ATTier::Upper;
        state_.at_games_left = weightedGames(config_.initial_games);
        rerollATTableAndPattern();
        out.push_back({EventType::UpperComeback, config_.upper_comeback_games, "64G comeback success -> upper AT restart"});
    } else {
        out.push_back({EventType::ATEnd, config_.upper_comeback_games, "64G comeback failed"});
        rerollNormalModeAndPattern();
    }
    return out;
}

void SlotEngine::endAT(std::vector<Event>& out) {
    if (state_.stocks > 0) {
        --state_.stocks;
        state_.at_games_left = weightedGames(config_.initial_games);
        rerollATTableAndPattern();
        out.push_back({EventType::ATStart, state_.at_games_left, "stock activated; common initial-game lottery; table/pattern re-roll"});
        return;
    }

    if (state_.at_tier == ATTier::Upper) {
        state_.in_at = false;
        auto c = resolveUpperComeback();
        out.insert(out.end(), c.begin(), c.end());
        return;
    }

    state_.in_at = false;
    out.push_back({EventType::ATEnd, 0, "AT ended"});
    rerollNormalModeAndPattern();
}

std::vector<Event> SlotEngine::spinAT() {
    std::vector<Event> out;
    if (!state_.in_at) return out;

    state_.last_reel_role = ReelRole::Miss;
    state_.last_reel_payout = 0;
    ++state_.total_games;
    if (state_.at_games_left <= 0) {
        endAT(out);
        return out;
    }

    --state_.at_games_left;
    const int net = state_.at_tier == ATTier::Upper ? config_.upper_net_per_game
                  : (state_.at_tier == ATTier::Middle ? config_.middle_net_per_game : config_.lower_net_per_game);
    applySectionDelta(net, out);

    if (chance(config_.at_table_pattern_move_rate)) rerollATTableAndPattern();

    auto ev = resolveATEvent();
    out.insert(out.end(), ev.begin(), ev.end());

    if (state_.in_at && state_.at_games_left <= 0) endAT(out);
    return out;
}

const char* SlotEngine::normalModeName(NormalMode v) {
    switch (v) {
        case NormalMode::NormalA: return "normal_a";
        case NormalMode::NormalB: return "normal_b";
        case NormalMode::Heaven: return "heaven";
        case NormalMode::SuperHeaven: return "super_heaven";
        case NormalMode::Special: return "special";
    }
    return "unknown";
}
const char* SlotEngine::atTierName(ATTier v) {
    switch (v) {
        case ATTier::Lower: return "lower";
        case ATTier::Middle: return "middle";
        case ATTier::Upper: return "upper";
    }
    return "unknown";
}
const char* SlotEngine::atTableName(ATTable v) {
    switch (v) {
        case ATTable::Normal: return "normal";
        case ATTable::Heaven: return "heaven";
        case ATTable::SuperHeaven: return "super_heaven";
        case ATTable::Specialized: return "specialized";
    }
    return "unknown";
}
const char* SlotEngine::reelRoleName(ReelRole v) {
    switch (v) {
        case ReelRole::Miss: return "miss";
        case ReelRole::OneMedal: return "one_medal";
        case ReelRole::Bell9: return "bell9";
        case ReelRole::Bell15: return "bell15";
        case ReelRole::Replay: return "replay";
    }
    return "miss";
}

const char* SlotEngine::eventName(EventType v) {
    switch (v) {
        case EventType::None: return "none";
        case EventType::CZ: return "cz";
        case EventType::Bonus: return "bonus";
        case EventType::EpisodeBonus: return "episode_bonus";
        case EventType::ATStart: return "at_start";
        case EventType::ATAddGames: return "at_add_games";
        case EventType::SpecialZone: return "special_zone";
        case EventType::UpperSpecialZone: return "upper_special_zone";
        case EventType::StockGain: return "stock_gain";
        case EventType::TierUp: return "tier_up";
        case EventType::TierDown: return "tier_down";
        case EventType::ATEnd: return "at_end";
        case EventType::UpperComeback: return "upper_comeback";
        case EventType::Freeze: return "freeze";
        case EventType::SectionCross: return "section_cross";
    }
    return "unknown";
}

std::string SlotEngine::stateJson() const {
    std::ostringstream o;
    o << "{\"setting\":" << state_.setting
      << ",\"targetPayoutRatio\":" << config_.target_payout_ratio
      << ",\"normalMode\":\"" << normalModeName(state_.normal_mode) << "\""
      << ",\"normalPattern\":" << state_.normal_pattern + 1
      // normalGames は既存UI互換のため表示回転数を返す。
      << ",\"normalGames\":" << state_.normal_display_games
      << ",\"normalActualGames\":" << state_.normal_actual_games
      << ",\"normalDisplayGames\":" << state_.normal_display_games
      << ",\"highProbabilityUnlocked\":" << (canEnterHighProbability() ? "true" : "false")
      << ",\"normalCeiling\":" << state_.normal_ceiling
      << ",\"inAT\":" << (state_.in_at ? "true" : "false")
      << ",\"atTier\":\"" << atTierName(state_.at_tier) << "\""
      << ",\"atTable\":\"" << atTableName(state_.at_table) << "\""
      << ",\"atPattern\":" << state_.at_pattern + 1
      << ",\"atGamesLeft\":" << state_.at_games_left
      << ",\"stocks\":" << state_.stocks
      << ",\"sectionDiff\":" << state_.section_diff
      << ",\"sectionMinDiff\":" << state_.section_min_diff
      << ",\"sectionCount\":" << state_.section_count
      << ",\"totalDiff\":" << state_.total_diff
      << ",\"totalGames\":" << state_.total_games
      << ",\"lastReelRole\":\"" << reelRoleName(state_.last_reel_role) << "\""
      << ",\"lastReelPayout\":" << state_.last_reel_payout
      << ",\"roleRemainderCount\":" << ROLE_REMAINDER_COUNT << "}";
    return o.str();
}

std::string SlotEngine::eventsJson(const std::vector<Event>& events, const MachineState& state) {
    std::ostringstream o;
    o << "{\"events\":[";
    for (std::size_t i = 0; i < events.size(); ++i) {
        if (i) o << ',';
        o << "{\"type\":\"" << eventName(events[i].type) << "\",\"value\":" << events[i].value
          << ",\"note\":\"" << escapeJson(events[i].note) << "\"}";
    }
    o << "],\"inAT\":" << (state.in_at ? "true" : "false")
      << ",\"gamesLeft\":" << state.at_games_left
      << ",\"stocks\":" << state.stocks
      << ",\"reelRole\":\"" << reelRoleName(state.last_reel_role) << "\""
      << ",\"reelPayout\":" << state.last_reel_payout << "}";
    return o.str();
}

std::string SlotEngine::spinNormalJson() { return eventsJson(spinNormal(), state_); }
std::string SlotEngine::spinATJson() { return eventsJson(spinAT(), state_); }

std::string SlotEngine::applyReelPayoutJson(int medals) {
    std::vector<Event> out;
    if (medals > 0) {
        applySectionDelta(medals, out);
    }
    return eventsJson(out, state_);
}

} // namespace schatgpt
