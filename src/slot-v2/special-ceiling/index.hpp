#pragma once
#include <stdint.h>
#include "../shared/rng.hpp"

namespace slotv2::special_ceiling {

enum class Ceiling : uint16_t {
    G777 = 777,
    G1500 = 1500
};

enum class G777Result : uint8_t {
    PlainAT = 0,
    MiddleStockDirect = 1,
    UpperDirect = 2
};

struct Result {
    Ceiling ceiling{Ceiling::G777};
    G777Result g777_result{G777Result::PlainAT};
    bool freeze{false};
};

// 特殊モード:
// 777G 95% / 1500G 5%。
// 777G時は AT / 1/8192相当 / 1/32768相当 を 1:1:1。
// 通常抽選の進行とは別系統。
Result draw(Rng& rng);

} // namespace slotv2::special_ceiling
