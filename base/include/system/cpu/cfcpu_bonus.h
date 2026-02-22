/**
 * @file cfcpu_bonus.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   These features might be unavalible, or unaccessible
 *          on some platforms or archs, so, make it as platforms
 * @version 0.1
 * @date 2026-02-22
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "base/expected/expected.hpp"
#include "base/span/span.h"
#include <cstdint>
#include <optional>
#include <string_view>

namespace cf {

enum class CPUBonusInfoViewError { NoError, GeneralError };

struct CPUBonusInfoView {
    cf::span<const std::string_view> features;
    cf::span<const uint32_t> cache_size;

    bool has_big_little = false;
    uint32_t big_core_count = 0;
    uint32_t little_core_count = 0;

    std::optional<uint16_t> temperature;
};

expected<CPUBonusInfoView, CPUBonusInfoViewError> getCPUBonusInfo(bool force_refresh = false);

} // namespace cf
