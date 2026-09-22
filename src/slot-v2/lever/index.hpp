#pragma once
#include "../shared/rng.hpp"
#include "../shared/types.hpp"

namespace slotv2::lever {

// LEVER ON の唯一の入口。
// allow_special=true の時だけ特殊直撃抽選を先に行い、当選時は本抽選へ割り込み優先する。
// 特殊直撃は通常時限定なので、runtimeがArea::Normalの時だけtrueを渡す。
LeverResult pull(Rng& rng, bool allow_special = true);

} // namespace slotv2::lever
