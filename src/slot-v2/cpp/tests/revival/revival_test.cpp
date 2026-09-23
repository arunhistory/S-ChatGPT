#include <cmath>
#include <iostream>
#include "revival-lottery/index.hpp"
#include "revival-cycle/index.hpp"
#include "revival-state/index.hpp"

namespace {

uint32_t countHits(slotv2::RoleFlag role) {
    uint32_t hits = 0u;
    for (uint16_t roll = 0; roll < 1000u; ++roll) {
        hits += slotv2::revival_lottery::fromRoll(role, roll) ? 1u : 0u;
    }
    return hits;
}

}

int main() {
    bool ok = true;

    ok = ok && countHits(slotv2::RoleFlag::Bell9) == 400u;
    ok = ok && countHits(slotv2::RoleFlag::Bell15) == 400u;
    ok = ok && countHits(slotv2::RoleFlag::Replay) == 601u;
    ok = ok && countHits(slotv2::RoleFlag::WeakChance) == 650u;
    ok = ok && countHits(slotv2::RoleFlag::Watermelon) == 700u;
    ok = ok && countHits(slotv2::RoleFlag::StrongChance) == 800u;
    ok = ok && countHits(slotv2::RoleFlag::WeakCherry) == 850u;
    ok = ok && countHits(slotv2::RoleFlag::StrongCherry) == 999u;

    ok = ok && countHits(slotv2::RoleFlag::OneMedal) == 0u;
    ok = ok && countHits(slotv2::RoleFlag::Miss) == 0u;
    ok = ok && countHits(slotv2::RoleFlag::PenguinChance) == 0u;

    // 指定された通常小役確率を使った5Gトータル期待度は約33%。
    const double per_game =
        ((1.0 / 15.0) + (1.0 / 80.0)) * 0.400
        + (1.0 / 30.0) * 0.601
        + (1.0 / 90.0) * 0.650
        + (1.0 / 100.0) * 0.700
        + (1.0 / 150.0) * 0.800
        + (1.0 / 180.0) * 0.850
        + (1.0 / 1000.0) * 0.999;

    const double five_game =
        1.0 - std::pow(1.0 - per_game, 5.0);

    ok = ok && five_game > 0.329 && five_game < 0.331;

    // 復活成功: 終了したTierを100Gで復活。
    {
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);

        slotv2::revival_state::State revival{};
        slotv2::revival_state::start(
            revival,
            slotv2::at_state::Tier::Upper
        );
        slotv2::revival_state::recordKickedNormalHit(revival);

        slotv2::revival_cycle::Game game{};
        game.active = true;
        game.role = slotv2::RoleFlag::StrongCherry;
        game.games_before = 5u;
        game.games_after = 4u;
        game.revival_hit = true;

        const auto r = slotv2::revival_cycle::finalizeGame(
            machine,
            revival,
            game
        );

        ok = ok && r.outcome == slotv2::revival_cycle::Outcome::Revived;
        ok = ok && r.tier == slotv2::at_state::Tier::Upper;
        ok = ok && r.stock_added;
        ok = ok && machine.area == slotv2::machine_state::Area::AT;
        ok = ok && machine.at.active;
        ok = ok && machine.at.tier == slotv2::at_state::Tier::Upper;
        ok = ok && machine.at.games_left == 100;
        ok = ok && machine.stock.count == 1u;
        ok = ok && !revival.active;
    }

    // 5G失敗: 通常へ。
    {
        slotv2::Rng rng(0x515151ULL);
        slotv2::machine_state::State machine{};
        slotv2::machine_state::reset(machine);

        slotv2::revival_state::State revival{};
        slotv2::revival_state::start(
            revival,
            slotv2::at_state::Tier::Middle
        );

        slotv2::revival_cycle::FinalizeResult last{};

        for (int i = 0; i < 5; ++i) {
            // 1枚役は復活抽選対象外なので確実に失敗。
            const auto game = slotv2::revival_cycle::beginGame(
                rng,
                revival,
                slotv2::RoleFlag::OneMedal
            );

            last = slotv2::revival_cycle::finalizeGame(
                machine,
                revival,
                game
            );
        }

        ok = ok && last.outcome == slotv2::revival_cycle::Outcome::Failed;
        ok = ok && machine.area == slotv2::machine_state::Area::Normal;
        ok = ok && !revival.active;
    }

    if (!ok) {
        std::cerr << "slot_v2_revival_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_revival_test: OK five_game="
              << five_game << "\n";
    return 0;
}
