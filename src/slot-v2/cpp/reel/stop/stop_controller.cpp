#include "index.hpp"
#include "../stop-first/index.hpp"
#include "../stop-second/index.hpp"
#include "../stop-third/index.hpp"

namespace slotv2::stop_controller {

stop_shared::Result resolve(const stop_shared::Context& ctx) {
    switch (ctx.stop_order) {
        case 0: return stop_first::resolve(ctx);
        case 1: return stop_second::resolve(ctx);
        case 2: return stop_third::resolve(ctx);
        default:
            return {stop_shared::ResolveStatus::InvalidReel, ctx.pressed_position, 0};
    }
}

} // namespace slotv2::stop_controller
