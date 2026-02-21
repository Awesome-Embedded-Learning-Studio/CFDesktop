#include "system/cpu/cfcpu.h"
#include <gtest/gtest.h>

TEST(cpu_query, cpu_query_successable) {
    auto cpuInfoQuery = getCPUInfo();
    EXPECT_TRUE(cpuInfoQuery.has_value());
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
