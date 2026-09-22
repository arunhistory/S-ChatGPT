#pragma once
#include <stdint.h>

namespace slotv2 {

static constexpr uint32_t kRngSpace = 134217728u; // 2^27
static constexpr int kReelSize = 21;
static constexpr int kMaxSlip = 4;

enum class CommandStatus : uint8_t {
    Ok = 0,
    RejectedPhase = 1
};

enum class SpecialHit : uint8_t {
    None = 0,
    MiddleATStock = 1, // 1/8192
    UpperAT = 2,       // 1/32768
    Freeze = 3         // 1/134217728
};

enum class RoleFlag : uint8_t {
    None = 0,
    Miss,
    OneMedal,
    Bell9,
    Bell15,
    Replay,
    WeakCherry,
    StrongCherry,
    Watermelon,
    WeakChance,
    StrongChance,
    PenguinChance,
    EntryAT,
    EntryBonus
};

enum class ReelId : uint8_t { Left = 0, Middle = 1, Right = 2 };

enum class Symbol : uint8_t {
    Unknown = 0,
    Red7,
    Blue7,
    Bar,
    Bell,
    Replay,
    Cherry,
    Watermelon,
    Penguin,
    Snow
};

struct LeverResult {
    CommandStatus command_status{CommandStatus::Ok};
    SpecialHit special{SpecialHit::None};
    RoleFlag role{RoleFlag::None};
    bool main_lottery_ran{false};
    bool entry_wait{false};
};

struct StopCandidates {
    uint8_t position[5]{0,0,0,0,0};
    uint8_t count{0};
};

} // namespace slotv2
