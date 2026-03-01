/**
 * @file cpu_profile.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief CPU Profile Information Query Implementation (Linux)
 * @version 0.1
 * @date 2026-02-22
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "cpu_profile.h"
#include "base/linux/proc_parser.h"

#include <cstdio>
#include <fstream>
#include <string_view>
#include <thread>
#include <unistd.h>

namespace {

// Configurable CPU usage sampling delay (milliseconds)
constexpr int CPU_USAGE_SAMPLE_DELAY_MS = 100;

// Read CPU frequency from sysfs file (kHz to Hz conversion)
uint32_t readCpuFreq(const char* path) noexcept {
    if (auto freq_khz = cf::read_uint32_file(path)) {
        return *freq_khz * 1000; // Convert kHz to Hz
    }
    return 0;
}

// Returns CPU usage percentage (0-100)
// Note: This function blocks for CPU_USAGE_SAMPLE_DELAY_MS milliseconds
float getCpuUsage() noexcept {
    FILE* stat = fopen("/proc/stat", "r");
    if (!stat) {
        return 0.0f;
    }

    // Parse first line: "cpu  user nice system idle iowait irq softirq"
    uint64_t user1 = 0, nice1 = 0, system1 = 0, idle1 = 0, iowait1 = 0, irq1 = 0, softirq1 = 0;
    if (fscanf(stat, "cpu %lu %lu %lu %lu %lu %lu %lu", &user1, &nice1, &system1, &idle1, &iowait1,
               &irq1, &softirq1) != 7) {
        fclose(stat);
        return 0.0f;
    }
    fclose(stat);

    const uint64_t idleTotal1 = idle1 + iowait1;
    const uint64_t total1 = user1 + nice1 + system1 + idle1 + iowait1 + irq1 + softirq1;

    // Sleep briefly to get delta
    std::this_thread::sleep_for(std::chrono::milliseconds(CPU_USAGE_SAMPLE_DELAY_MS));

    // Re-read
    stat = fopen("/proc/stat", "r");
    if (!stat) {
        return 0.0f;
    }

    uint64_t user2 = 0, nice2 = 0, system2 = 0, idle2 = 0, iowait2 = 0, irq2 = 0, softirq2 = 0;
    if (fscanf(stat, "cpu %lu %lu %lu %lu %lu %lu %lu", &user2, &nice2, &system2, &idle2, &iowait2,
               &irq2, &softirq2) != 7) {
        fclose(stat);
        return 0.0f;
    }
    fclose(stat);

    const uint64_t idleTotal2 = idle2 + iowait2;
    const uint64_t total2 = user2 + nice2 + system2 + idle2 + iowait2 + irq2 + softirq2;

    const uint64_t totalDelta = total2 - total1;
    const uint64_t idleDelta = idleTotal2 - idleTotal1;

    if (totalDelta == 0) {
        return 0.0f;
    }

    return 100.0f * (1.0f - static_cast<float>(idleDelta) / static_cast<float>(totalDelta));
}

} // namespace

cf::expected<cf::CPUProfileInfo, cf::CPUProfileInfoError> query_cpu_profile_info() {
    cf::CPUProfileInfo profile_info{};

    // Get logical and physical core counts from /proc/cpuinfo
    std::ifstream cpuinfo("/proc/cpuinfo");
    if (cpuinfo.is_open()) {
        std::string line;
        int logicalCount = 0;
        int physicalCount = 0;
        bool foundPhysicalCores = false;

        while (std::getline(cpuinfo, line)) {
            const std::string_view line_sv(line);

            // Count "processor" entries for logical cores
            if (line_sv.find("processor") == 0) {
                const std::string_view numStr = cf::parse_cpuinfo_field(line_sv, "processor");
                if (!numStr.empty()) {
                    if (auto num = cf::parse_uint32(numStr)) {
                        logicalCount = static_cast<int>(*num) + 1;
                    }
                }
            }

            // Get "cpu cores" for physical cores (same for all cores, read first occurrence)
            if (!foundPhysicalCores) {
                const std::string_view coresStr = cf::parse_cpuinfo_field(line_sv, "cpu cores");
                if (!coresStr.empty()) {
                    if (auto cores = cf::parse_uint32(coresStr)) {
                        physicalCount = static_cast<int>(*cores);
                        foundPhysicalCores = true;
                    }
                }
            }
        }

        // Fallback: if physical cores not found, use logical count
        if (physicalCount == 0) {
            physicalCount = logicalCount;
        }

        // Also try sysconf for logical count as fallback
        if (logicalCount == 0) {
            logicalCount = sysconf(_SC_NPROCESSORS_ONLN);
            if (logicalCount < 0) {
                logicalCount = 1;
            }
        }

        profile_info.logical_cnt = static_cast<uint16_t>(logicalCount);
        profile_info.physical_cnt = static_cast<uint16_t>(physicalCount);
    } else {
        // Fallback to sysconf
        int logicalCount = sysconf(_SC_NPROCESSORS_ONLN);
        if (logicalCount < 0) {
            logicalCount = 1;
        }
        profile_info.logical_cnt = static_cast<uint16_t>(logicalCount);
        profile_info.physical_cnt = static_cast<uint16_t>(logicalCount);
    }

    // Get frequency from cpufreq sysfs
    profile_info.current_frequecy =
        readCpuFreq("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq");
    profile_info.max_frequency =
        readCpuFreq("/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq");

    // If cpufreq is not available, try cpuinfo_max_freq
    if (profile_info.max_frequency == 0) {
        profile_info.max_frequency =
            readCpuFreq("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq");
    }

    // Get CPU usage
    profile_info.cpu_usage_percentage = getCpuUsage();

    return profile_info;
}
