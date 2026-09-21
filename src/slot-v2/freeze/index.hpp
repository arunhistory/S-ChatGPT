#pragma once
#include "../shared/types.hpp"

namespace slotv2::freeze {

struct Directive {
    bool active;
    Symbol target_symbol;
    bool special_presentation_required;
};

// 抽選は special-lottery が担当。
// freeze は当選後のC++内部制御だけを担当し、逆回転などの見せ方はTS側へ渡す。
Directive begin(SpecialHit hit);

} // namespace slotv2::freeze
