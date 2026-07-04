/**
 * @file desktop/main/early_session/early_handle/early_handle.cpp
 * @brief Implementation of EarlyHandle.
 */
#include "early_handle.h"

namespace cf::desktop::early_stage {
void EarlyHandle::setEarlySettingsHandle(std::unique_ptr<early_settings::EarlySettings> settings) {
    this->settings = std::move(settings);
}

std::unique_ptr<early_settings::EarlySettings> EarlyHandle::unlockEarlySettings() {
    return std::move(settings);
}

const early_settings::EarlySettings& EarlyHandle::early_settings() const {
    return *settings;
}

} // namespace cf::desktop::early_stage
