#pragma once
#include <stdint.h>
#include "shared/types.hpp"
#include "normal/normal_latent.hpp"
#include "normal/normal_flow.hpp"
#include "normal/normal_high.hpp"
#include "normal/normal_role_trigger.hpp"
#include "normal/normal_ceiling.hpp"
#include "normal/normal_mode.hpp"
#include "at/at_event.hpp"
#include "core/pending_event.hpp"

namespace slotv2::debug {

// Typed native-only injection catalogue. Future UI can enumerate all entries
// instead of inventing display symbols or modifying real C++ probability
// tables. Pending is an expert channel: the owning state must support it.
enum class Channel : uint8_t {
    None = 0,
    Role = 1,
    Special = 2,
    NormalFlow = 3,
    NormalHigh = 4,
    NormalRole = 5,
    NormalCeiling = 6,
    ATEvent = 7,
    CZOutcome = 8,
    Pending = 9,
    Mode = 10,
    Presentation = 11
};
enum class CZOutcome : uint8_t {
    Hit = 1, Miss = 2, ThirdMiss = 3
};
struct Descriptor {
    Channel channel;
    uint32_t value;
    const char* name;
};
#define SLOT_DEBUG_FLAG(channel_, val_) \
    Descriptor{Channel::channel_, static_cast<uint32_t>(val_), #val_}

inline constexpr Descriptor kCatalogue[] = {
    SLOT_DEBUG_FLAG(Role, RoleFlag::None),
    SLOT_DEBUG_FLAG(Role, RoleFlag::Miss),
    SLOT_DEBUG_FLAG(Role, RoleFlag::OneMedal),
    SLOT_DEBUG_FLAG(Role, RoleFlag::Bell9),
    SLOT_DEBUG_FLAG(Role, RoleFlag::Bell15),
    SLOT_DEBUG_FLAG(Role, RoleFlag::Replay),
    SLOT_DEBUG_FLAG(Role, RoleFlag::WeakCherry),
    SLOT_DEBUG_FLAG(Role, RoleFlag::StrongCherry),
    SLOT_DEBUG_FLAG(Role, RoleFlag::Watermelon),
    SLOT_DEBUG_FLAG(Role, RoleFlag::WeakChance),
    SLOT_DEBUG_FLAG(Role, RoleFlag::StrongChance),
    SLOT_DEBUG_FLAG(Role, RoleFlag::PenguinChance),
    SLOT_DEBUG_FLAG(Role, RoleFlag::EntryAT),
    SLOT_DEBUG_FLAG(Role, RoleFlag::EntryBonus),

    SLOT_DEBUG_FLAG(Special, SpecialHit::MiddleATStock),
    SLOT_DEBUG_FLAG(Special, SpecialHit::UpperAT),
    SLOT_DEBUG_FLAG(Special, SpecialHit::Freeze),

    SLOT_DEBUG_FLAG(NormalFlow, normal_flow::Reward::LowerAT),
    SLOT_DEBUG_FLAG(NormalFlow, normal_flow::Reward::Bonus),
    SLOT_DEBUG_FLAG(NormalFlow, normal_flow::Reward::CZ),
    SLOT_DEBUG_FLAG(NormalHigh, normal_high::Reward::CZ),
    SLOT_DEBUG_FLAG(NormalHigh, normal_high::Reward::Bonus),
    SLOT_DEBUG_FLAG(NormalRole, normal_role_trigger::DrawResult::Bonus),
    SLOT_DEBUG_FLAG(NormalRole, normal_role_trigger::DrawResult::LowerAT),
    SLOT_DEBUG_FLAG(NormalRole, normal_role_trigger::DrawResult::MiddleAT),

    SLOT_DEBUG_FLAG(NormalCeiling, normal_ceiling::Reward::CZ),
    SLOT_DEBUG_FLAG(NormalCeiling, normal_ceiling::Reward::Bonus),
    SLOT_DEBUG_FLAG(NormalCeiling, normal_ceiling::Reward::LowerAT),
    SLOT_DEBUG_FLAG(NormalCeiling, normal_ceiling::Reward::LowerATWithStock),
    SLOT_DEBUG_FLAG(NormalCeiling, normal_ceiling::Reward::Freeze),

    SLOT_DEBUG_FLAG(ATEvent, at_event::Hit),
    SLOT_DEBUG_FLAG(ATEvent, at_event::Fall),
    SLOT_DEBUG_FLAG(ATEvent, at_event::AddGames),
    SLOT_DEBUG_FLAG(ATEvent, at_event::Special),
    SLOT_DEBUG_FLAG(ATEvent, at_event::Episode),
    SLOT_DEBUG_FLAG(ATEvent, at_event::UpperSpecial),
    SLOT_DEBUG_FLAG(ATEvent, at_event::ChainZone),

    SLOT_DEBUG_FLAG(CZOutcome, CZOutcome::Hit),
    SLOT_DEBUG_FLAG(CZOutcome, CZOutcome::Miss),
    SLOT_DEBUG_FLAG(CZOutcome, CZOutcome::ThirdMiss),

    SLOT_DEBUG_FLAG(Pending, pending_event::BellFiveAT),
    SLOT_DEBUG_FLAG(Pending, pending_event::CZThreeMissHit),
    SLOT_DEBUG_FLAG(Pending, pending_event::NextHitAT),
    SLOT_DEBUG_FLAG(Pending, pending_event::ATHit),
    SLOT_DEBUG_FLAG(Pending, pending_event::ATFall),
    SLOT_DEBUG_FLAG(Pending, pending_event::ATAddGames),
    SLOT_DEBUG_FLAG(Pending, pending_event::ATSpecial),
    SLOT_DEBUG_FLAG(Pending, pending_event::ATEpisode),
    SLOT_DEBUG_FLAG(Pending, pending_event::ATUpperSpecial),
    SLOT_DEBUG_FLAG(Pending, pending_event::ATMultiple),
    SLOT_DEBUG_FLAG(Pending, pending_event::SectionTierUp),
    SLOT_DEBUG_FLAG(Pending, pending_event::SectionSpecial),
    SLOT_DEBUG_FLAG(Pending, pending_event::SectionUpperSpec),
    SLOT_DEBUG_FLAG(Pending, pending_event::CZHit),
    SLOT_DEBUG_FLAG(Pending, pending_event::ATWindowEmpty),
    SLOT_DEBUG_FLAG(Pending, pending_event::ATStockAvailable),
    SLOT_DEBUG_FLAG(Pending, pending_event::BonusComplete),
    SLOT_DEBUG_FLAG(Pending, pending_event::BonusEpisodeUpgrade),
    SLOT_DEBUG_FLAG(Pending, pending_event::UpperComebackHit),
    SLOT_DEBUG_FLAG(Pending, pending_event::SpecialZoneAddGames),
    SLOT_DEBUG_FLAG(Pending, pending_event::SpecialZoneBonus),
    SLOT_DEBUG_FLAG(Pending, pending_event::ATChainZone),

    SLOT_DEBUG_FLAG(Mode, normal_mode::Mode::NormalA),
    SLOT_DEBUG_FLAG(Mode, normal_mode::Mode::NormalB),
    SLOT_DEBUG_FLAG(Mode, normal_mode::Mode::Heaven),
    SLOT_DEBUG_FLAG(Mode, normal_mode::Mode::SuperHeaven),
    SLOT_DEBUG_FLAG(Mode, normal_mode::Mode::Special),

    SLOT_DEBUG_FLAG(Presentation, normal_latent::Route::Direct),
    SLOT_DEBUG_FLAG(Presentation, normal_latent::Route::Omen),
    SLOT_DEBUG_FLAG(Presentation, normal_latent::Route::OmenCZ)
};
#undef SLOT_DEBUG_FLAG

inline constexpr uint32_t kCatalogueCount =
    static_cast<uint32_t>(sizeof(kCatalogue) / sizeof(kCatalogue[0]));

inline bool valid(Channel channel, uint32_t value) {
    for (const auto& flag : kCatalogue) {
        if (flag.channel == channel && flag.value == value) return true;
    }
    return false;
}

struct Request {
    Channel channel{Channel::None};
    uint32_t value{0};
};
struct State {
    Request next{};
    Request last_applied{};
    bool next_armed{false};
    bool presentation_armed{false};
    uint8_t presentation{normal_latent::kRandomRoute};
};

} // namespace slotv2::debug
