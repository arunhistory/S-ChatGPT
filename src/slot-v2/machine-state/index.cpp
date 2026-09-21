#include "index.hpp"

namespace slotv2::machine_state {

void reset(State& state) {
    state = {};
    state.area = Area::Normal;
}

} // namespace slotv2::machine_state
