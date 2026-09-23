#include "reel/stop/reel_candidate.hpp"

namespace slotv2::reel_candidate {

reel_strip::StripView middle() {
    return reel_strip::get(ReelId::Middle);
}

reel_strip::StripView right() {
    return reel_strip::get(ReelId::Right);
}

} // namespace slotv2::reel_candidate
