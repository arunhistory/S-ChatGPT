#pragma once
#include "../shared/rng.hpp"
#include "../shared/types.hpp"

namespace slotv2::lever {

// LEVER ON の唯一の入口。
// 特殊直撃抽選を先に行い、当選時は本抽選へ割り込み優先する。
LeverResult pull(Rng& rng);

} // namespace slotv2::lever
