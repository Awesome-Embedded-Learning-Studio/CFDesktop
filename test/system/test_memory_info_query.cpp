#include "system/memory/memory_info.h"
#include <gtest/gtest.h>
using namespace cf;

TEST(memory_query, system_memory_query_successable) {
    MemoryInfo info;
    getSystemMemoryInfo(info);
    EXPECT_GT(info.physical.total_bytes, 0);
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
