#include "index.hpp"

namespace slotv2::main_lottery {

static constexpr uint32_t kOneMedal = 107374182u; // ~4/5
static constexpr uint32_t kBell9 = 8947849u;      // ~1/15
static constexpr uint32_t kBell15 = 1677722u;     // ~1/80
static constexpr uint32_t kReplay = 4473924u;     // ~1/30
static constexpr uint32_t kWeakCherry = 745654u;  // ~1/180
static constexpr uint32_t kStrongCherry = 134218u;// ~1/1000
static constexpr uint32_t kWatermelon = 1342177u; // ~1/100
static constexpr uint32_t kWeakChance = 1491308u; // ~1/90
static constexpr uint32_t kStrongChance = 894785u;// ~1/150
static constexpr uint32_t kPenguin = 268435u;     // ~1/500

static constexpr uint32_t kTotal =
    kOneMedal + kBell9 + kBell15 + kReplay +
    kWeakCherry + kStrongCherry + kWatermelon +
    kWeakChance + kStrongChance + kPenguin;

static_assert(kTotal <= kRngSpace);

RoleFlag draw(uint32_t d) {
    d &= (kRngSpace - 1u);

    if (d < kOneMedal) return RoleFlag::OneMedal; d -= kOneMedal;
    if (d < kBell9) return RoleFlag::Bell9; d -= kBell9;
    if (d < kBell15) return RoleFlag::Bell15; d -= kBell15;
    if (d < kReplay) return RoleFlag::Replay; d -= kReplay;
    if (d < kWeakCherry) return RoleFlag::WeakCherry; d -= kWeakCherry;
    if (d < kStrongCherry) return RoleFlag::StrongCherry; d -= kStrongCherry;
    if (d < kWatermelon) return RoleFlag::Watermelon; d -= kWatermelon;
    if (d < kWeakChance) return RoleFlag::WeakChance; d -= kWeakChance;
    if (d < kStrongChance) return RoleFlag::StrongChance; d -= kStrongChance;
    if (d < kPenguin) return RoleFlag::PenguinChance;
    return RoleFlag::Miss;
}

} // namespace slotv2::main_lottery
