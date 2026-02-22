/**
 * @file cpu_features.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief CPU Feature Detection Implementation (Linux)
 * @version 0.1
 * @date 2026-02-22
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "cpu_features.h"

#if defined(__x86_64__) || defined(__i386__)
#include <cpuid.h>
#else
#include "base/linux/proc_parser.h"
#include <fstream>
#endif

#include <string>
#include <vector>

namespace {

#if defined(__x86_64__) || defined(__i386__)
// x86/x64_64 CPUID based feature detection

inline unsigned long long read_xcr0() noexcept {
    unsigned long long xcr0;
    __asm__ __volatile__("xgetbv" : "=a"(xcr0) : "c"(0) : "edx");
    return xcr0;
}

void addFeatureIfSupported(bool condition, const char* name, std::vector<std::string>& feats) {
    if (condition) {
        feats.emplace_back(name);
    }
}

void query_x86_features(std::vector<std::string>& feats) {
    unsigned int eax, ebx, ecx, edx;

    // CPUID leaf 1, subleaf 0
    if (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
        // SSE / AES / FMA from ECX
        addFeatureIfSupported(ecx & (1 << 25), "aes", feats);
        addFeatureIfSupported(ecx & (1 << 12), "fma", feats);

        // AVX needs OS support
        bool osxsave = ecx & (1 << 27);
        bool avx_support = ecx & (1 << 28);

        if (osxsave && avx_support) {
            unsigned long long xcr = read_xcr0();
            if ((xcr & 0x6) == 0x6) {
                feats.emplace_back("avx");
            }
        }
    }

    // CPUID leaf 7, subleaf 0
    if (__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx)) {
        addFeatureIfSupported(ebx & (1 << 5), "avx2", feats);
        addFeatureIfSupported(ebx & (1 << 16), "avx512f", feats);
        addFeatureIfSupported(ebx & (1 << 29), "sha_ni", feats);

        // Check for specific AVX-512 extensions
        if (ebx & (1 << 16)) {
            addFeatureIfSupported(ebx & (1 << 17), "avx512dq", feats);
            addFeatureIfSupported(ebx & (1 << 28), "avx512cd", feats);
            addFeatureIfSupported(ebx & (1 << 30), "avx512bw", feats);
            addFeatureIfSupported(ebx & (1 << 31), "avx512vl", feats);
        }
    }
}

#elif defined(__aarch64__) || defined(__arm__)
// ARM feature detection from /proc/cpuinfo

// Helper: Parse space-separated features from a string_view (zero-copy)
void parse_features_line(std::string_view features_str, std::vector<std::string>& feats) {
    size_t start = 0;
    while (start < features_str.size()) {
        // Skip whitespace
        start = features_str.find_first_not_of(" \t", start);
        if (start == std::string_view::npos) {
            break;
        }

        // Find end of feature name
        const size_t end = features_str.find_first_of(" \t", start);
        if (end == std::string_view::npos) {
            // Last feature - take the rest
            const std::string_view feature = cf::trim_whitespace(features_str.substr(start));
            if (!feature.empty()) {
                feats.emplace_back(feature);
            }
            break;
        }

        // Extract feature
        const std::string_view feature = features_str.substr(start, end - start);
        if (!feature.empty()) {
            feats.emplace_back(feature);
        }

        start = end + 1;
    }
}

void query_arm_features(std::vector<std::string>& feats) {
    std::ifstream cpuinfo("/proc/cpuinfo");
    if (!cpuinfo.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(cpuinfo, line)) {
        const std::string_view line_sv(line);

        // Check for "Features" line in ARM cpuinfo
        if (line_sv.find("Features") == 0 || line_sv.find("flags") == 0) {
            const std::string_view features = cf::parse_cpuinfo_field(line_sv, "Features");
            if (features.empty()) {
                // Try "flags" field
                const std::string_view flags = cf::parse_cpuinfo_field(line_sv, "flags");
                if (!flags.empty()) {
                    parse_features_line(flags, feats);
                }
            } else {
                parse_features_line(features, feats);
            }
            break;
        }
    }
}

#else
// Other architectures - minimal implementation
void query_generic_features(std::vector<std::string>& feats) {
    std::ifstream cpuinfo("/proc/cpuinfo");
    if (!cpuinfo.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(cpuinfo, line)) {
        const std::string_view line_sv(line);

        if (line_sv.find("flags") == 0 || line_sv.find("Features") == 0) {
            const std::string_view features = cf::parse_cpuinfo_field(line_sv, "flags");
            if (features.empty()) {
                const std::string_view features_alt = cf::parse_cpuinfo_field(line_sv, "Features");
                if (!features_alt.empty()) {
                    // Re-use ARM parser
                    parse_features_line(features_alt, feats);
                }
            } else {
                parse_features_line(features, feats);
            }
            break;
        }
    }
}

#endif

} // namespace

void query_cpu_features(std::vector<std::string>& feats) {
#if defined(__x86_64__) || defined(__i386__)
    query_x86_features(feats);
#elif defined(__aarch64__) || defined(__arm__)
    query_arm_features(feats);
#else
    query_generic_features(feats);
#endif
}
