#include "reel/stop/stop_candidate.hpp"

namespace slotv2::stop_candidate {

StopCandidates build(uint8_t pressed_position) {
    StopCandidates out{};
    const int base = static_cast<int>(pressed_position % kReelSize);

    for (int slip = 0; slip <= kMaxSlip; ++slip) {
        int p = base - slip;
        while (p < 0) p += kReelSize;
        out.position[out.count++] = static_cast<uint8_t>(p);
    }
    return out;
}

} // namespace slotv2::stop_candidate
