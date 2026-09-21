#include "index.hpp"

namespace slotv2::freeze {

Directive begin(SpecialHit hit) {
    if (hit != SpecialHit::Freeze) {
        return {false, Symbol::Unknown, false};
    }
    return {true, Symbol::Blue7, true};
}

} // namespace slotv2::freeze
