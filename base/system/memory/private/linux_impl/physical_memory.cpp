/**
 * @file physical_memory.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Physical Memory Query Implementation (Linux)
 * @version 0.1
 * @date 2026-02-27
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "physical_memory.h"

#include <cstdio>
#include <cstdint>
#include <cstring>

namespace cf {
namespace linux_impl {

namespace {

/**
 * @brief Parse a value from /proc/meminfo line.
 * Format: "FieldName:    12345 kB"
 */
bool parseMemInfoLine(const char* line, const char* fieldName, uint64_t& outKb) {
    size_t fieldNameLen = strlen(fieldName);
    if (strncmp(line, fieldName, fieldNameLen) != 0) {
        return false;
    }

    // Skip to the number after the colon
    const char* p = line + fieldNameLen;
    while (*p == ':' || *p == ' ' || *p == '\t') {
        p++;
    }

    if (*p == '\0') {
        return false;
    }

    // Parse the number
    char* end;
    unsigned long value = strtoul(p, &end, 10);
    if (end == p) {
        return false;
    }

    outKb = static_cast<uint64_t>(value);
    return true;
}

} // anonymous namespace

void queryPhysicalMemory(PhysicalMemory& physical) {
    FILE* fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        // Set to zero on failure
        physical.total_bytes = 0;
        physical.available_bytes = 0;
        physical.free_bytes = 0;
        return;
    }

    uint64_t memTotal = 0;
    uint64_t memAvailable = 0;
    uint64_t memFree = 0;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        uint64_t value;
        if (parseMemInfoLine(line, "MemTotal", value)) {
            memTotal = value;
        } else if (parseMemInfoLine(line, "MemAvailable", value)) {
            memAvailable = value;
        } else if (parseMemInfoLine(line, "MemFree", value)) {
            memFree = value;
        }

        // Break early if we have all values
        if (memTotal > 0 && memAvailable > 0 && memFree > 0) {
            break;
        }
    }

    fclose(fp);

    // Convert KB to bytes
    physical.total_bytes = memTotal * 1024;
    physical.available_bytes = memAvailable * 1024;
    physical.free_bytes = memFree * 1024;
}

} // namespace linux_impl
} // namespace cf
