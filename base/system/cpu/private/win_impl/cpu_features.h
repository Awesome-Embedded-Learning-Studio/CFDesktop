/**
 * @file cpu_features.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief CPU Feature Detection (SSE, AVX, AES, etc.)
 * @version 0.1
 * @date 2026-02-22
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include <string>
#include <vector>

/**
 * @brief Internal Windows Query for CPU features
 *
 * @param feats Output parameter to store detected CPU feature names
 * @return cf::expected<void, cf::CPUBonusInfoViewError>
 */
void query_cpu_features(std::vector<std::string>& feats);