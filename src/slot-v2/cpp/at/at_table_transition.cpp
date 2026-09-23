#include "at/at_table_transition.hpp"

namespace slotv2::at_table_transition {

at_state::Table fromRoll(
    at_state::Table current,
    uint16_t roll_0_to_99
) {
    const uint16_t roll = static_cast<uint16_t>(roll_0_to_99 % 100u);

    if (current == at_state::Table::Heaven
        || current == at_state::Table::SuperHeaven) {
        if (roll < 30u) return at_state::Table::Normal;       // 30%
        if (roll < 55u) return at_state::Table::Heaven;       // 25%
        if (roll < 75u) return at_state::Table::SuperHeaven;  // 20%
        return at_state::Table::Specialized;                  // 25%
    }

    if (roll < 50u) return at_state::Table::Normal;           // 50%
    if (roll < 75u) return at_state::Table::Heaven;           // 25%
    return at_state::Table::Specialized;                      // 25%
}

at_state::Table draw(
    Rng& rng,
    at_state::Table current
) {
    return fromRoll(
        current,
        static_cast<uint16_t>(rng.uniformBelow(100u))
    );
}

} // namespace slotv2::at_table_transition
