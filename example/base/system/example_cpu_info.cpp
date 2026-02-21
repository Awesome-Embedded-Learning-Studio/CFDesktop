#include "system/cpu/cfcpu.h"
#include "system/cpu/cfcpu_profile.h"
#include <iostream>

using namespace cf;

int main() {
    // Main Actions
    auto cpuInfoQuery = getCPUInfo();
    if (!cpuInfoQuery.has_value()) {
        std::cerr << "Welp, Failed to query cpu info" << (int)cpuInfoQuery.error() << "\n";
    }

    std::cout << "Get the CPU Info: \n"
              << "\t" << cpuInfoQuery->arch << "\n"
              << "\t" << cpuInfoQuery->manufacturer << "\n"
              << "\t" << cpuInfoQuery->model << "\n";

    auto cpuProfileInfo = getCPUProfileInfo();
    if (!cpuProfileInfo.has_value()) {
        std::cerr << "Failed to query cpu profile info\n";
        return 1;
    }

    std::cout << "\nGet the CPU Profile Info: \n"
              << "\tLogical Cores: " << (int)cpuProfileInfo->logical_cnt << "\n"
              << "\tPhysical Cores: " << (int)cpuProfileInfo->physical_cnt << "\n"
              << "\tCurrent Frequency: " << cpuProfileInfo->current_frequecy << " MHz\n"
              << "\tMax Frequency: " << cpuProfileInfo->max_frequency << " MHz\n"
              << "\tCPU Usage: " << cpuProfileInfo->cpu_usage_percentage << "%\n";

    return 0;
}
