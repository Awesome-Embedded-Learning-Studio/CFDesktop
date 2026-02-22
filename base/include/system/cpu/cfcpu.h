/**
 * @file cpu.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief CPU Info Fetch
 * @version 0.1
 * @date 2026-02-21
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include "base/expected/expected.hpp"
#include <string_view>

namespace cf {
enum class CPUInfoErrorType { CPU_QUERY_NOERROR, CPU_QUERY_GENERAL_FAILED };

struct CPUInfoView {
    std::string_view model;
    std::string_view arch;
    std::string_view manufacturer;
};

expected<CPUInfoView, CPUInfoErrorType> getCPUInfo(bool force_refresh = false);

} // namespace cf
