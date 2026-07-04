/**
 * @file swap_memory.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Swap Space Query Implementation (Linux)
 * @version 0.1
 * @date 2026-02-27
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "swap_memory.h"

#include <cstdint>
#include <cstdio>
#include <cstring>

namespace cf {
namespace linux_impl {

namespace {

bool parseMemInfoLine(const char* line, const char* fieldName, uint64_t& outKb) {
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

void querySwapMemory(SwapMemory& swap) {
    FILE* fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        swap.total_bytes = 0;
        swap.free_bytes = 0;
        return;
    }

    uint64_t swapTotal = 0;
    uint64_t swapFree = 0;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        uint64_t value;
        if (parseMemInfoLine(line, "SwapTotal", value)) {
            swapTotal = value;
        } else if (parseMemInfoLine(line, "SwapFree", value)) {
            swapFree = value;
        }

        if (swapTotal > 0 && swapFree > 0) {
            break;
        }
    }

    fclose(fp);

    swap.total_bytes = swapTotal * 1024;
    swap.free_bytes = swapFree * 1024;
}

} // namespace linux_impl
} // namespace cf
