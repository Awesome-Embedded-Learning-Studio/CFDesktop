#include "cpu_bonus.h"
#include "cpu_features.h"
#include "cpu_host.h"
#include <windows.h>
#include <cstdint>

// MinGW may not have EfficiencyClass in PROCESSOR_RELATIONSHIP (Windows 10 1903+)
#ifndef PROCESSOR_RELATIONSHIP_EFFICIENCY_CLASS
// Offset of EfficiencyClass in PROCESSOR_RELATIONSHIP structure
#define PROCESSOR_RELATIONSHIP_EFFICIENCY_CLASS 20
#endif

namespace {

// Helper to get EfficiencyClass - works around missing MinGW header definition
inline BYTE GetProcessorEfficiencyClass(const PROCESSOR_RELATIONSHIP* proc) {
    // Access EfficiencyClass by offset since MinGW may not have the field
    // The EfficiencyClass is at offset 20 (after GroupCount and GroupMask array)
    return *reinterpret_cast<const BYTE*>(reinterpret_cast<const uint8_t*>(proc) + PROCESSOR_RELATIONSHIP_EFFICIENCY_CLASS);
}

void filledCache(cf::CPUBonusInfoHost& host) {
    host.cache_size.resize(3);
    DWORD len = 0;
    GetLogicalProcessorInformationEx(RelationCache, nullptr, &len);

    std::vector<uint8_t> buffer(len);

    if (!GetLogicalProcessorInformationEx(
            RelationCache, (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)buffer.data(), &len))
        return;

    auto ptr = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)buffer.data();
    auto end = buffer.data() + len;

    while ((uint8_t*)ptr < end) {
        auto& c = ptr->Cache;
        uint32_t kb = c.CacheSize / 1024;

        if (c.Level == 1)
            host.cache_size[0] += kb;
        if (c.Level == 2)
            host.cache_size[1] += kb;
        if (c.Level == 3)
            host.cache_size[2] += kb;

        ptr = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)((uint8_t*)ptr + ptr->Size);
    }
}

void getCPUEffecientClass(cf::CPUBonusInfoHost& host) {
    DWORD len = 0;
    GetLogicalProcessorInformationEx(RelationProcessorCore, nullptr, &len);

    std::vector<uint8_t> buffer(len);

    if (!GetLogicalProcessorInformationEx(
            RelationProcessorCore, (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)buffer.data(), &len))
        return;

    auto ptr = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)buffer.data();
    auto end = buffer.data() + len;

    while ((uint8_t*)ptr < end) {
        if (GetProcessorEfficiencyClass(&ptr->Processor) == 0)
            host.big_core_count++;
        else
            host.little_core_count++;

        ptr = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)((uint8_t*)ptr + ptr->Size);
    }

    host.has_big_little = (host.big_core_count > 0 && host.little_core_count > 0);
}

} // namespace

cf::expected<void, cf::CPUBonusInfoViewError> query_cpu_bonus_info(cf::CPUBonusInfoHost& bonus) {
    query_cpu_features(bonus.features);
    filledCache(bonus);
    bonus.temperature = {}; // Not accessible in Windows
    getCPUEffecientClass(bonus);

    return {};
}
