#include "system/cpu/cfcpu.h"
#include "system/cpu/cfcpu_bonus.h"
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
              << "\t" << std::string{cpuInfoQuery->arch} << "\n"
              << "\t" << std::string{cpuInfoQuery->manufacturer} << "\n"
              << "\t" << std::string{cpuInfoQuery->model} << "\n";

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

    auto cpuBonusInfo = getCPUBonusInfo();
    if (!cpuBonusInfo.has_value()) {
        std::cerr << "Failed to query cpu bonus info\n";
        return 1;
    }

    std::cout << "\nGet the CPU Bonus Info: \n";
    std::cout << "\tFeatures: ";
    for (const auto& feature : cpuBonusInfo->features) {
        std::cout << std::string{feature} << " ";
    }
    std::cout << "\n";

    if (cpuBonusInfo->has_big_little) {
        std::cout << "\tBig.Little Architecture: Yes\n"
                  << "\t\tBig Cores: " << cpuBonusInfo->big_core_count << "\n"
                  << "\t\tLittle Cores: " << cpuBonusInfo->little_core_count << "\n";
    } else {
        std::cout << "\tBig.Little Architecture: No\n";
    }

    if (cpuBonusInfo->temperature.has_value()) {
        std::cout << "\tTemperature: " << *cpuBonusInfo->temperature << "\n";
    } else {
        std::cout << "\tTemperature: Not available\n";
    }

    return 0;
}
