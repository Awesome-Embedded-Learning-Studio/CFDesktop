#include "system/cpu/cfcpu.h"
#include "system/cpu/cfcpu_profile.h"
#include <gtest/gtest.h>
using namespace cf;

TEST(cpu_query, cpu_query_successable) {
    auto cpuInfoQuery = getCPUInfo();
    EXPECT_TRUE(cpuInfoQuery.has_value());
}

TEST(cpu_performace_query, cpu_query_successable) {
    auto cpuInfoQuery = getCPUProfileInfo();
    EXPECT_TRUE(cpuInfoQuery.has_value());
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
