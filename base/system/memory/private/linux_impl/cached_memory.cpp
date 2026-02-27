/**
 * @file cached_memory.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Linux-specific Cached Memory Query Implementation
 * @version 0.1
 * @date 2026-02-27
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "cached_memory.h"

#include <cstdio>
#include <cstdint>
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

void queryCachedMemory(CachedMemory& cached) {
    FILE* fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        cached.buffers_bytes = 0;
        cached.cached_bytes = 0;
        cached.shared_bytes = 0;
        cached.slab_bytes = 0;
        return;
    }

    uint64_t buffers = 0;
    uint64_t cachedMem = 0;
    uint64_t shmem = 0;
    uint64_t slab = 0;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        uint64_t value;
        if (parseMemInfoLine(line, "Buffers", value)) {
            buffers = value;
        } else if (parseMemInfoLine(line, "Cached", value)) {
            // Note: There may also be "SReclaimable:" which is similar to Cached
            // For simplicity, we only get the main Cached value
            cachedMem = value;
        } else if (parseMemInfoLine(line, "Shmem", value)) {
            shmem = value;
        } else if (parseMemInfoLine(line, "Slab", value)) {
            slab = value;
        }

        if (buffers > 0 && cachedMem > 0 && shmem > 0 && slab > 0) {
            break;
        }
    }

    fclose(fp);

    // Convert KB to bytes
    cached.buffers_bytes = buffers * 1024;
    cached.cached_bytes = cachedMem * 1024;
    cached.shared_bytes = shmem * 1024;
    cached.slab_bytes = slab * 1024;
}

} // namespace linux_impl
} // namespace cf
