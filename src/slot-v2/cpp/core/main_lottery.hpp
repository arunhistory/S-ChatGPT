#pragma once
#include "shared/types.hpp"

namespace slotv2::main_lottery {

// 特殊直撃が非当選だった時だけ呼ぶ本抽選。
RoleFlag draw(uint32_t draw27);

} // namespace slotv2::main_lottery
