#pragma once
#include "../shared/rng.hpp"

namespace slotv2::cz_lottery {

static constexpr uint32_t kBaseDenominator = 100u;

// CZ基本抽選 1/100。
// 役による変動は役別補正値が未確定なので別レイヤーへ残す。
bool drawBase(Rng& rng);

} // namespace slotv2::cz_lottery
