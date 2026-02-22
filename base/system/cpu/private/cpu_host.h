/**
 * @file cpu_host.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief As Recommends, do not use this as directly
 * @version 0.1
 * @date 2026-02-21
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include <optional>
#include <string>
#include <vector>
namespace cf {
struct CPUInfoHost {
  public:
    std::string model;
    std::string manufest;
    std::string arch;
};

struct CPUBonusInfoHost {
    std::vector<std::string> features;
    std::vector<uint32_t> cache_size;

    bool has_big_little = false;
    uint32_t big_core_count = 0;
    uint32_t little_core_count = 0;

    std::optional<uint16_t> temperature;
};

} // namespace cf