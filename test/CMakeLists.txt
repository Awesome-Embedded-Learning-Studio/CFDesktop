# Include custom CMake helper functions
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# GoogleTest for unit testing
include(FetchContent)

message(STATUS "Fetching GoogleTest...")
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
    GIT_SHALLOW TRUE
)

# For Windows: Prevent overriding parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

message(STATUS "GoogleTest ${gtest_VERSION} configured")

# Enable CTest integration
enable_testing()

# Include GoogleTest in our tests
# We need to link against the gtest and gtest_main libraries
# and use the GTest::gtest and GTest::gtest_main targets

# Add boot_test subdirectory (legacy tests)
# add_subdirectory(boot_test)  # Moved below to avoid order issues

# Add base unit tests subdirectory
add_subdirectory(base)

# Add System Test
add_subdirectory(system)

# Add boot_test subdirectory (legacy tests)
add_subdirectory(boot_test)

log_info("TestFramework" "All test modules configured")
