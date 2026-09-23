#include <iostream>
#include "special/chain_zone.hpp"
#include "core/main_lottery.hpp"

int main() {
    bool ok = true;
    slotv2::Rng rng(0x535031585FULL);

    ok = ok && slotv2::chain_zone::episodeThresholdPerThousand(1u)==0u;
    ok = ok && slotv2::chain_zone::episodeThresholdPerThousand(2u)==50u;
    ok = ok && slotv2::chain_zone::episodeThresholdPerThousand(3u)==75u;
    ok = ok && slotv2::chain_zone::episodeThresholdPerThousand(4u)==100u;

    uint64_t signals=0;
    constexpr uint64_t trials=1000000;
    for (uint64_t i=0; i<trials; ++i) {
        const auto role=slotv2::main_lottery::draw(rng.next27());
        const auto roll=static_cast<uint8_t>(rng.uniformBelow(9u));
        signals+=slotv2::chain_zone::continuationFromRoll(role,roll);
    }
    const double ratio=static_cast<double>(signals)/trials;
    ok=ok && ratio>0.097 && ratio<0.103;

    slotv2::chain_zone::State s{};
    slotv2::chain_zone::start(s);
    ok=ok && s.active && s.games_left==5u;
    slotv2::chain_zone::Step last{};
    for (int i=0;i<5;++i)
        last=slotv2::chain_zone::playOne(
            s,rng,slotv2::RoleFlag::OneMedal
        );
    ok=ok && last.ended && !s.active && last.earned_bonuses==0u;
    s.bonus_remaining=4u;
    uint32_t released=0u, episodes=0u;
    for (int i=0;i<4;++i) {
        const auto batch=slotv2::chain_zone::releaseNext(s,rng);
        released+=batch.queued;
        episodes+=batch.episode;
        ok=ok && batch.multi && batch.batch_size==4u;
    }
    ok=ok && released==4u && episodes<=1u && s.bonus_remaining==0u
        && !slotv2::chain_zone::releaseNext(s,rng).queued;

    if (!ok) {
        std::cerr << "slot_v2_chain_zone_test: FAILED ratio=" << ratio << "\n";
        return 1;
    }
    std::cout << "slot_v2_chain_zone_test: OK signal=" << ratio << "\n";
    return 0;
}
