/**
 * @file process_memory.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Process Memory Usage Query Implementation (Linux)
 * @version 0.1
 * @date 2026-02-27
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "process_memory.h"

#include <cstdio>
#include <cstdint>
#include <cstring>

namespace cf {
namespace linux_impl {

namespace {

/**
 * @brief Parse a value from /proc/self/status line.
 * Format: "VmRSS:     12345 kB"
 */
bool parseStatusLine(const char* line, const char* fieldName, uint64_t& outKb) {
    size_t fieldNameLen = strlen(fieldName);
    if (strncmp(line, fieldName, fieldNameLen) != 0) {
        return false;
    }

    const char* p = line + fieldNameLen;
    while (*p == ':' || *p == ' ' || *p == '\t') {
        p++;
    }

    if (*p == '\0') {
        return false;
    }

    char* end;
    unsigned long value = strtoul(p, &end, 10);
    if (end == p) {
        return false;
    }

    outKb = static_cast<uint64_t>(value);
    return true;
}

} // anonymous namespace

void queryProcessMemory(ProcessMemory& process) {
    FILE* fp = fopen("/proc/self/status", "r");
    if (!fp) {
        process.vm_rss_bytes = 0;
        process.vm_size_bytes = 0;
        process.vm_peak_bytes = 0;
        return;
    }

    uint64_t vmRSS = 0;
    uint64_t vmSize = 0;
    uint64_t vmPeak = 0;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        uint64_t value;
        if (parseStatusLine(line, "VmRSS", value)) {
            vmRSS = value;
        } else if (parseStatusLine(line, "VmSize", value)) {
            vmSize = value;
        } else if (parseStatusLine(line, "VmPeak", value)) {
            vmPeak = value;
        }

        if (vmRSS > 0 && vmSize > 0 && vmPeak > 0) {
            break;
        }
    }

    fclose(fp);

    // Convert KB to bytes
    process.vm_rss_bytes = vmRSS * 1024;
    process.vm_size_bytes = vmSize * 1024;
    process.vm_peak_bytes = vmPeak * 1024;
}

} // namespace linux_impl
} // namespace cf
