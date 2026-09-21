#include "index.hpp"

namespace slotv2::special_result {

Result resolve(SpecialHit hit) {
    switch (hit) {
        case SpecialHit::MiddleATStock:
            return {
                SpecialHit::MiddleATStock,
                EntryTarget::MiddleAT,
                1,
                false
            };

        case SpecialHit::UpperAT:
            return {
                SpecialHit::UpperAT,
                EntryTarget::UpperAT,
                0,
                false
            };

        case SpecialHit::Freeze:
            return {
                SpecialHit::Freeze,
                EntryTarget::UpperAT,
                1,
                true
            };

        case SpecialHit::None:
        default:
            return {};
    }
}

} // namespace slotv2::special_result
