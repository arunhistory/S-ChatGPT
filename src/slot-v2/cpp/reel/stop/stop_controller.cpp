#include "reel/stop/stop_controller.hpp"
#include "reel/stop/stop_first.hpp"
#include "reel/stop/stop_second.hpp"
#include "reel/stop/stop_third.hpp"

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
