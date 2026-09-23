#include "reel/reel_acquisition.hpp"
#include "reel/reel_line.hpp"
#include "reel/stop/bell_shape.hpp"

namespace slotv2::acquisition {
Result judge(
    RoleFlag internal_role,
    uint8_t left_pos,
    uint8_t middle_pos,
    uint8_t right_pos,
    bool had_substitute,
    bool had_role_miss,
    bool had_assist_gap
) {
    if (had_assist_gap) {
        return {internal_role, Status::AssistGap, 0};
    }
    if (had_substitute) {
        return {internal_role, Status::Substitute, 0};
    }
    if (had_role_miss) {
        return {internal_role, Status::Missed, 0};
    }

    const auto line = line::read(left_pos, middle_pos, right_pos);
    if (!line.ready) return {internal_role, Status::NotReady, 0};

    switch (internal_role) {
        case RoleFlag::Bell9:
            if (bell_shape::matchesFinal(
                    internal_role, left_pos, middle_pos, right_pos)) {
                return {internal_role, Status::Acquired, 9};
            }
            return {internal_role, Status::Missed, 0};

        case RoleFlag::Bell15:
            if (bell_shape::matchesFinal(
                    internal_role, left_pos, middle_pos, right_pos)) {
                return {internal_role, Status::Acquired, 15};
            }
            return {internal_role, Status::Missed, 0};

        case RoleFlag::ThreeMedal:
            if (bell_shape::matchesFinal(
                    internal_role, left_pos, middle_pos, right_pos)) {
                return {internal_role, Status::Acquired, 3};
            }
            return {internal_role, Status::Missed, 0};

        case RoleFlag::Replay:
            if (line.left == Symbol::Replay
                && line.middle == Symbol::Replay
                && line.right == Symbol::Replay) {
                // REPLAYは再遊技。メダル払出とは分離する。
                return {internal_role, Status::Acquired, 0};
            }
            return {internal_role, Status::Missed, 0};

        case RoleFlag::WeakCherry:
        case RoleFlag::StrongCherry:
        case RoleFlag::Watermelon:
        case RoleFlag::WeakChance:
        case RoleFlag::StrongChance:
        case RoleFlag::PenguinChance:
            // これらは停止形を役ごとに確定してから専用判定を追加する。
            return {internal_role, Status::NoPayoutRole, 0};

        default:
            return {internal_role, Status::NoPayoutRole, 0};
    }
}

} // namespace slotv2::acquisition
