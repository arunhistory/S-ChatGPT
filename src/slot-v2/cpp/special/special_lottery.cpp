#include "special/special_lottery.hpp"

namespace slotv2::special_lottery {

static constexpr uint32_t kFreezeCount = 1u;
static constexpr uint32_t kUpperCount = kRngSpace / 32768u; // 4096
static constexpr uint32_t kMiddleStockCount = kRngSpace / 8192u; // 16384

static_assert(kFreezeCount == 1u);
static_assert(kUpperCount == 4096u);
static_assert(kMiddleStockCount == 16384u);
static_assert(kFreezeCount + kUpperCount + kMiddleStockCount < kRngSpace);

SpecialHit draw(uint32_t draw27) {
    draw27 &= (kRngSpace - 1u);

    // 排他的な領域割当。3結果は同一の1回抽選から決まる。
    // 各結果の領域数をそのまま確率に対応させる。
    if (draw27 < kFreezeCount) return SpecialHit::Freeze;
    draw27 -= kFreezeCount;

    if (draw27 < kUpperCount) return SpecialHit::UpperAT;
    draw27 -= kUpperCount;

    if (draw27 < kMiddleStockCount) return SpecialHit::MiddleATStock;
    return SpecialHit::None;
}

} // namespace slotv2::special_lottery
