#include <iostream>
#include "core/runtime.hpp"
#include "core/setting_profile.hpp"

int main() {
    bool ok = true;

    slotv2::runtime::State state{};
    slotv2::runtime::reset(state, 0x53455454494E47ULL);

    ok = ok
        && slotv2::runtime::currentSetting(state)
            == slotv2::setting_profile::kDefaultSetting;

    for (uint8_t setting = slotv2::setting_profile::kMinSetting;
         setting <= slotv2::setting_profile::kMaxSetting;
         ++setting) {
        const auto status = slotv2::runtime::resetWithSetting(
            state,
            0x1000ULL + setting,
            setting
        );

        ok = ok
            && status == slotv2::runtime::SettingResetStatus::Applied;
        ok = ok
            && slotv2::runtime::currentSetting(state) == setting;
        ok = ok
            && state.machine.area == slotv2::machine_state::Area::Normal;
        ok = ok
            && state.session.phase == slotv2::session::Phase::Idle;
        ok = ok
            && state.accounting.total_bet == 0;
        ok = ok
            && state.accounting.total_payout == 0;
        ok = ok
            && state.accounting.total_diff == 0;
    }

    // Invalid setting requests must not mutate the running state.
    (void)slotv2::runtime::resetWithSetting(
        state,
        0x7777ULL,
        4u
    );
    state.accounting.total_bet = 123;
    state.machine.area = slotv2::machine_state::Area::AT;

    const auto before_setting = slotv2::runtime::currentSetting(state);
    const auto before_bet = state.accounting.total_bet;
    const auto before_area = state.machine.area;

    const auto invalid_low = slotv2::runtime::resetWithSetting(
        state,
        0x8888ULL,
        0u
    );

    ok = ok
        && invalid_low
            == slotv2::runtime::SettingResetStatus::InvalidSetting;
    ok = ok && slotv2::runtime::currentSetting(state) == before_setting;
    ok = ok && state.accounting.total_bet == before_bet;
    ok = ok && state.machine.area == before_area;

    const auto invalid_high = slotv2::runtime::resetWithSetting(
        state,
        0x9999ULL,
        8u
    );

    ok = ok
        && invalid_high
            == slotv2::runtime::SettingResetStatus::InvalidSetting;
    ok = ok && slotv2::runtime::currentSetting(state) == before_setting;
    ok = ok && state.accounting.total_bet == before_bet;
    ok = ok && state.machine.area == before_area;

    ok = ok && !slotv2::setting_profile::validSetting(0u);
    ok = ok && slotv2::setting_profile::validSetting(1u);
    ok = ok && slotv2::setting_profile::validSetting(7u);
    ok = ok && !slotv2::setting_profile::validSetting(8u);

    if (!ok) {
        std::cerr << "slot_v2_setting_change_test: FAILED\n";
        return 1;
    }

    std::cout << "slot_v2_setting_change_test: OK\n";
    return 0;
}
