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
#include <string>

struct CPUInfoHost {
  public:
    std::string model;
    std::string manufest;
    std::string arch;
};