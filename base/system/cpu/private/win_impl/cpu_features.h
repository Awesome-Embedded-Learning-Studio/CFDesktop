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
 * @brief Internal Windows Query for CPU features.
 *
 * @param[out] feats Output parameter to store detected CPU feature names.
 *
 * @throws     None (error reporting via return code if applicable).
 *
 * @note       None.
 *
 * @warning    None.
 *
 * @since      0.1
 * @ingroup    system_cpu
 */
void query_cpu_features(std::vector<std::string>& feats);