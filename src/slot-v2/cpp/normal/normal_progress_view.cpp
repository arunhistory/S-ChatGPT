#include "index.hpp"

namespace slotv2::normal_progress_view {

uint32_t pack(const normal_progress::State& state) {
    return static_cast<uint32_t>(state.cz_misses)
        | (static_cast<uint32_t>(state.normal_hits_without_at) << 8)
        | (static_cast<uint32_t>(state.bell9_streak) << 16)
        | (state.next_hit_at_guaranteed ? (1u << 24) : 0u);
}

} // namespace slotv2::normal_progress_view
