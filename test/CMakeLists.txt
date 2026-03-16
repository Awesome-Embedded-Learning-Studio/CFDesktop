# Include custom CMake helper functions
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include third party helper from root cmake directory
include(${CMAKE_SOURCE_DIR}/cmake/third_party_helper.cmake)

# =============================================================================
# GoogleTest for unit testing
# =============================================================================
# Setup GoogleTest from third_party or download if not available
setup_third_party(
    NAME           googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)

# For Windows: Prevent overriding parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

message(STATUS "GoogleTest configured")

# Enable CTest integration
enable_testing()

# Add base unit tests subdirectory
add_subdirectory(base)

# Add logger tests subdirectory
add_subdirectory(logger)

# Add System Test
add_subdirectory(system)

# Add ui tests
add_subdirectory(ui)

# Add boot_test subdirectory (legacy tests)
add_subdirectory(boot_test)

log_info("TestFramework" "All test modules configured")
