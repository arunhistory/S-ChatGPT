#include "index.hpp"

namespace slotv2::special_result {

Result resolve(SpecialHit hit) {
    switch (hit) {
        case SpecialHit::MiddleATStock:
            return {
                SpecialHit::MiddleATStock,
                EntryTarget::MiddleAT,
                stock_count_lottery::Profile::Middle,
                false
            };

        case SpecialHit::UpperAT:
            return {
                SpecialHit::UpperAT,
                EntryTarget::UpperAT,
                stock_count_lottery::Profile::None,
                false
            };

        case SpecialHit::Freeze:
            return {
                SpecialHit::Freeze,
                EntryTarget::UpperAT,
                stock_count_lottery::Profile::Upper,
                true
            };

        case SpecialHit::None:
        default:
            return {};
    }
}

} // namespace slotv2::special_result
