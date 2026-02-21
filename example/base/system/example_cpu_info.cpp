#include "system/cpu/cfcpu.h"
#include <iostream>
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
}
