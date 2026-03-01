/**
 * @file cpu_bonus.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief CPU Bonus Information Query Implementation (Linux)
 * @version 0.1
 * @date 2026-02-22
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "cpu_bonus.h"
#include "base/linux/proc_parser.h"
#include "cpu_features.h"

#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <optional>
#include <string_view>
#include <unistd.h>

namespace {

// Buffer size for sysfs paths
// Need space for: /sys/class/thermal/ (18) + d_name (255) + /type or /temp (5) + null
constexpr size_t PATH_BUFFER_SIZE = 320;

uint32_t readCacheSize(int cpu, int level) noexcept {
    char path[PATH_BUFFER_SIZE];
    snprintf(path, sizeof(path), "/sys/devices/system/cpu/cpu%d/cache/index%d/size", cpu, level);

    FILE* file = fopen(path, "r");
    if (!file) {
        return 0;
    }

    char sizeStr[32];
    if (fgets(sizeStr, sizeof(sizeStr), file) == nullptr) {
        fclose(file);
        return 0;
    }
    fclose(file);

    // Parse size (e.g., "32K" or "1M") using modern utilities
    const std::string_view size_sv(sizeStr);
    if (auto size = cf::parse_cache_size(size_sv)) {
        return *size;
    }
    return 0;
}

void fillCacheSizes(cf::CPUBonusInfoHost& host) {
    host.cache_size.resize(3); // L1, L2, L3

    // Read from CPU0 (typical for all cores)
    for (int level = 0; level < 3; ++level) {
        uint32_t size = readCacheSize(0, level);
        host.cache_size[level] = size;
    }
}

void detectBigLittleCores(cf::CPUBonusInfoHost& host) {
    // Method 1: Check CPU capacities from /sys/devices/system/cpu/cpu*/cpu_capacity
    bool hasCapacityInfo = false;

    int cpuCount = sysconf(_SC_NPROCESSORS_ONLN);
    if (cpuCount < 0) {
        cpuCount = 1;
    }

    for (int cpu = 0; cpu < cpuCount; ++cpu) {
        char capacityPath[PATH_BUFFER_SIZE];
        snprintf(capacityPath, sizeof(capacityPath), "/sys/devices/system/cpu/cpu%d/cpu_capacity",
                 cpu);

        if (auto capacity = cf::read_uint32_file(capacityPath)) {
            hasCapacityInfo = true;
            // Determine threshold for big vs little (typically around 512-600)
            // Big cores have higher capacity values
            if (*capacity > 550) {
                host.big_core_count++;
            } else {
                host.little_core_count++;
            }
        } else {
            break; // No more CPUs with capacity info
        }
    }

    if (hasCapacityInfo) {
        host.has_big_little = (host.big_core_count > 0 && host.little_core_count > 0);
        return;
    }

    // Method 2: Check max frequencies of different CPUs
    uint32_t maxFreq = 0;
    uint32_t minFreq = UINT32_MAX;

    for (int cpu = 0; cpu < cpuCount; ++cpu) {
        char freqPath[PATH_BUFFER_SIZE];
        snprintf(freqPath, sizeof(freqPath),
                 "/sys/devices/system/cpu/cpu%d/cpufreq/cpuinfo_max_freq", cpu);

        if (auto freq = cf::read_uint32_file(freqPath)) {
            if (*freq > maxFreq) {
                maxFreq = *freq;
            }
            if (*freq < minFreq) {
                minFreq = *freq;
            }
        }
    }

    // If there's a significant frequency difference, we have big.LITTLE
    if (maxFreq > 0 && minFreq < UINT32_MAX && (maxFreq - minFreq) > 200000) {
        // Count cores at each frequency level
        for (int cpu = 0; cpu < cpuCount; ++cpu) {
            char freqPath[PATH_BUFFER_SIZE];
            snprintf(freqPath, sizeof(freqPath),
                     "/sys/devices/system/cpu/cpu%d/cpufreq/cpuinfo_max_freq", cpu);

            if (auto freq = cf::read_uint32_file(freqPath)) {
                if (*freq >= maxFreq - 100000) {
                    host.big_core_count++;
                } else {
                    host.little_core_count++;
                }
            }
        }

        host.has_big_little = (host.big_core_count > 0 && host.little_core_count > 0);
    }
}

std::optional<uint16_t> readCpuTemperature() noexcept {
    // Try to find CPU thermal zone
    const char* thermalBasePath = "/sys/class/thermal";

    DIR* dir = opendir(thermalBasePath);
    if (!dir) {
        return std::nullopt;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (strncmp(entry->d_name, "thermal_zone", 12) == 0) {
            char typePath[PATH_BUFFER_SIZE];
            snprintf(typePath, sizeof(typePath), "%s/%s/type", thermalBasePath, entry->d_name);

            FILE* typeFile = fopen(typePath, "r");
            if (typeFile) {
                char type[64];
                if (fgets(type, sizeof(type), typeFile)) {
                    // Remove trailing newline
                    size_t len = strlen(type);
                    if (len > 0 && type[len - 1] == '\n') {
                        type[len - 1] = '\0';
                    }

                    // Look for CPU-related thermal zones
                    const std::string_view type_sv(type);
                    constexpr std::string_view cpu_marker = "cpu";
                    constexpr std::string_view CPU_marker = "CPU";
                    constexpr std::string_view x86_marker = "x86";

                    if (type_sv.find(cpu_marker) != std::string_view::npos ||
                        type_sv.find(CPU_marker) != std::string_view::npos ||
                        type_sv.find(x86_marker) != std::string_view::npos) {

                        char tempPath[PATH_BUFFER_SIZE];
                        snprintf(tempPath, sizeof(tempPath), "%s/%s/temp", thermalBasePath,
                                 entry->d_name);

                        FILE* tempFile = fopen(tempPath, "r");
                        if (tempFile) {
                            int tempMillidegree = 0;
                            if (fscanf(tempFile, "%d", &tempMillidegree) == 1) {
                                fclose(tempFile);
                                closedir(dir);
                                return static_cast<uint16_t>(tempMillidegree /
                                                             1000); // Convert to Celsius
                            }
                            fclose(tempFile);
                        }
                    }
                }
                fclose(typeFile);
            }
        }
    }

    closedir(dir);
    return std::nullopt;
}

} // namespace

cf::expected<void, cf::CPUBonusInfoViewError> query_cpu_bonus_info(cf::CPUBonusInfoHost& bonus) {
    // Get CPU features
    query_cpu_features(bonus.features);

    // Get cache sizes
    fillCacheSizes(bonus);

    // Detect big.LITTLE architecture
    detectBigLittleCores(bonus);

    // Try to read CPU temperature
    bonus.temperature = readCpuTemperature();

    return {};
}
