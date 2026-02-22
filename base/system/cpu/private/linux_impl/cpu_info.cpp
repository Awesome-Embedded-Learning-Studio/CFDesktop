/**
 * @file cpu_info.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief CPU Basic Information Query Implementation (Linux)
 * @version 0.1
 * @date 2026-02-22
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "cpu_info.h"
#include "base/linux/proc_parser.h"

#include <fstream>
#include <string_view>
#include <sys/utsname.h>
#include <unistd.h>

cf::expected<void, cf::CPUInfoErrorType> query_cpu_basic_info(cf::CPUInfoHost& hostInfo) {
    std::ifstream cpuinfo("/proc/cpuinfo");
    if (!cpuinfo.is_open()) {
        return cf::unexpected(cf::CPUInfoErrorType::CPU_QUERY_GENERAL_FAILED);
    }

    std::string line;
    bool foundModel = false;
    bool foundVendor = false;

    while (std::getline(cpuinfo, line)) {
        const std::string_view line_sv(line);

        // Try to get model name
        if (!foundModel) {
            const std::string_view model = cf::parse_cpuinfo_field(line_sv, "model name");
            if (!model.empty()) {
                hostInfo.model = model;
                foundModel = true;
            }
        }

        // Try to get vendor ID (for x86/x64)
        if (!foundVendor) {
            const std::string_view vendor = cf::parse_cpuinfo_field(line_sv, "vendor_id");
            if (!vendor.empty()) {
                hostInfo.manufest = vendor;
                foundVendor = true;
            }
        }

        // For ARM, try to get vendor from "CPU implementer"
        if (!foundVendor) {
            const std::string_view implementer = cf::parse_cpuinfo_field(line_sv, "CPU implementer");
            if (!implementer.empty()) {
                // Parse hex implementer ID using modern utilities
                if (auto impl_val = cf::parse_hex_uint32(implementer)) {
                    const std::string_view vendor = cf::arm_implementer_to_vendor(*impl_val);
                    if (vendor != "Unknown") {
                        hostInfo.manufest = vendor;
                    } else {
                        // Fallback to raw hex string
                        hostInfo.manufest = implementer;
                    }
                } else {
                    hostInfo.manufest = implementer;
                }
                foundVendor = true;
            }
        }

        if (foundModel && foundVendor) {
            break;
        }
    }

    // If vendor not found in cpuinfo, try alternative sources
    if (!foundVendor) {
        hostInfo.manufest = "Unknown";
    }

    // Get architecture
    struct utsname unameInfo;
    if (uname(&unameInfo) == 0) {
        hostInfo.arch = unameInfo.machine;
    } else {
#ifdef __x86_64__
        hostInfo.arch = "x86_64";
#elif defined(__i386__)
        hostInfo.arch = "x86";
#elif defined(__aarch64__)
        hostInfo.arch = "aarch64";
#elif defined(__arm__)
        hostInfo.arch = "arm";
#elif defined(__riscv) && (__riscv_xlen == 64)
        hostInfo.arch = "riscv64";
#elif defined(__riscv) && (__riscv_xlen == 32)
        hostInfo.arch = "riscv32";
#else
        hostInfo.arch = "unknown";
#endif
    }

    return {};
}
