#include "cpu_bonus.h"
#include "cpu_host.h"
#include "cpu_features.h"
#include <windows.h>

namespace {
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
        if (ptr->Processor.EfficiencyClass == 0)
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
