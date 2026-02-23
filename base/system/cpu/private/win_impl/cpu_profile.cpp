/**
 * @file cpu_profile.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief CPU Profile Information Query Implementation
 * @version 0.1
 * @date 2026-02-22
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "cpu_profile.h"
#include <Windows.h>
#include <pdh.h>
#include <powrprof.h>
#include <vector>

#ifndef _PROCESSOR_POWER_INFORMATION_DEFINED
#    define _PROCESSOR_POWER_INFORMATION_DEFINED

typedef struct _PROCESSOR_POWER_INFORMATION {
    ULONG Number;
    ULONG MaxMhz;
    ULONG CurrentMhz;
    ULONG MhzLimit;
    ULONG MaxIdleState;
    ULONG CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, *PPROCESSOR_POWER_INFORMATION;

#endif

namespace {

float getCpuUsage() {
    PDH_HQUERY query;
    PDH_HCOUNTER counter;

    PdhOpenQuery(nullptr, 0, &query);
    PdhAddEnglishCounterW(query, L"\\Processor(_Total)\\% Processor Time", 0, &counter);
    PdhCollectQueryData(query);

    Sleep(100);

    PdhCollectQueryData(query);

    PDH_FMT_COUNTERVALUE value;
    PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, nullptr, &value);

    PdhCloseQuery(query);

    return static_cast<float>(value.doubleValue);
}

} // namespace

cf::expected<cf::CPUProfileInfo, cf::CPUProfileInfoError> query_cpu_profile_info() {
    cf::CPUProfileInfo profile_info{};

    // logical cnt
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    profile_info.logical_cnt = static_cast<uint8_t>(sysInfo.dwNumberOfProcessors);

    // physical cnt
    DWORD len = 0;
    GetLogicalProcessorInformationEx(RelationProcessorCore, nullptr, &len);
    std::vector<char> buffer;
    buffer.resize(len);
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX info =
        reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.data());

    if (!GetLogicalProcessorInformationEx(RelationProcessorCore, info, &len)) {
        profile_info.physical_cnt = 0;
    } else {
        uint8_t count = 0;
        char* ptr = buffer.data();
        while (ptr < buffer.data() + len) {
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX entry =
                reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(ptr);

            if (entry->Relationship == RelationProcessorCore)
                count++;

            ptr += entry->Size;
        }
        profile_info.physical_cnt = count;
    }

    PROCESSOR_POWER_INFORMATION power_info[64];

    ULONG retLen;
    CallNtPowerInformation(ProcessorInformation, nullptr, 0, power_info, sizeof(power_info));

    profile_info.current_frequecy = power_info[0].CurrentMhz * 1'000'000;
    profile_info.max_frequency = power_info[0].MaxMhz * 1'000'000;

    profile_info.cpu_usage_percentage = getCpuUsage();

    return profile_info;
}
