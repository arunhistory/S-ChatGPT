#include <iostream>
#include "core/runtime.hpp"
#include "core/setting_profile.hpp"

int main() {
    bool ok = true;
    slotv2::runtime::State state{};
    slotv2::runtime::reset(state, 0x53455454494E47ULL);

    ok = ok && slotv2::runtime::currentSetting(state) == 6u;
    ok = ok && slotv2::setting_profile::implementedSetting(6u);
    ok = ok && slotv2::at_state::kUpperNetPerGame == 9;

    const auto ready = slotv2::runtime::resetWithSetting(state, 0x1006ULL, 6u);
    ok = ok && ready == slotv2::runtime::SettingResetStatus::Applied;
    ok = ok && state.machine.area == slotv2::machine_state::Area::Normal;
    ok = ok && state.session.phase == slotv2::session::Phase::Idle;

    state.accounting.total_bet = 123;
    state.machine.area = slotv2::machine_state::Area::AT;
    for (uint8_t setting = 1; setting <= 7; ++setting) {
        ok = ok && slotv2::setting_profile::validSetting(setting);
        if (setting == 6u) continue;
        ok = ok && !slotv2::setting_profile::implementedSetting(setting);
        const auto raw = slotv2::setting_profile::normalRaw(setting);
        ok = ok && raw.at_threshold_27 == 0u
            && raw.bonus_threshold_27 == 0u && raw.cz_threshold_27 == 0u;
        const auto status = slotv2::runtime::resetWithSetting(
            state, 0x1000ULL + setting, setting
        );
        ok = ok && status == slotv2::runtime::SettingResetStatus::NotImplemented;
        ok = ok && slotv2::runtime::currentSetting(state) == 6u
            && state.accounting.total_bet == 123
            && state.machine.area == slotv2::machine_state::Area::AT;
    }

    for (uint8_t setting : {0u, 8u}) {
        const auto status = slotv2::runtime::resetWithSetting(
            state, 0x2000ULL + setting, setting
        );
        ok = ok && status == slotv2::runtime::SettingResetStatus::InvalidSetting;
        ok = ok && state.accounting.total_bet == 123
            && state.machine.area == slotv2::machine_state::Area::AT;
    }

    if (!ok) {
        std::cerr << "slot_v2_setting_change_test: FAILED\n";
        return 1;
    }
    std::cout << "slot_v2_setting_change_test: OK\n";
    return 0;
}
