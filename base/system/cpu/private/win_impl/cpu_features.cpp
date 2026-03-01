/**
 * @file cpu_features.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief CPU Feature Detection Implementation
 * @version 0.1
 * @date 2026-02-22
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "cpu_features.h"
#include <intrin.h>

namespace {
__attribute__((target("xsave"))) static unsigned long long read_xcr0() {
    return _xgetbv(0);
}
void addFeatureIfSupported(bool condition, const char* name, std::vector<std::string>& feats) {
    if (condition) {
        feats.emplace_back(name);
    }
}

} // namespace

/**
 * @brief Internal Windows Query for CPU features
 *
 * @param feats Output parameter to store detected CPU feature names
 * @return cf::expected<void, cf::CPUBonusInfoViewError>
 */
void query_cpu_features(std::vector<std::string>& feats) {
    int cpuInfo[4] = {};

    __cpuid(cpuInfo, 1);

    // SSE / AES / FMA
    addFeatureIfSupported(cpuInfo[2] & (1 << 25), "aes", feats);
    addFeatureIfSupported(cpuInfo[2] & (1 << 12), "fma", feats);

    // AVX 需要 OS 支持
    bool osxsave = cpuInfo[2] & (1 << 27);
    bool avx_support = cpuInfo[2] & (1 << 28);

    if (osxsave && avx_support) {
        unsigned long long xcr = read_xcr0();
        if ((xcr & 0x6) == 0x6)
            feats.emplace_back("avx");
    }

    __cpuidex(cpuInfo, 7, 0);

    addFeatureIfSupported(cpuInfo[1] & (1 << 5), "avx2", feats);
    addFeatureIfSupported(cpuInfo[1] & (1 << 16), "avx512", feats);
    addFeatureIfSupported(cpuInfo[1] & (1 << 29), "sha2", feats);
}