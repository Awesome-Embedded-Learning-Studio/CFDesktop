# Include custom CMake helper functions
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(add_gtest_executable)

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

# =============================================================================
# Qt6 Test for signal testing
# =============================================================================
find_package(Qt6 QUIET COMPONENTS Test)
if(Qt6Test_FOUND)
    message(STATUS "Qt6::Test found - signal tests enabled")
    set(QT_TEST_AVAILABLE TRUE)
else()
    message(STATUS "Qt6::Test not found - signal tests will be disabled")
    set(QT_TEST_AVAILABLE FALSE)
endif()

# NOTE: base foundation unit tests now live in the aex submodule
# (third_party/aex/test) and run from aex's own standalone build.

# Add config manager tests subdirectory
add_subdirectory(config_manager)

# Add logger tests subdirectory
add_subdirectory(logger)

# Add System Test
add_subdirectory(system)

# Add IPC tests subdirectory
add_subdirectory(ipc)

# NOTE: ui tests now live in the QuarkWidgets submodule (third_party/QuarkWidgets/test).

# Add desktop tests
add_subdirectory(desktop)

# Add boot_test subdirectory (legacy tests)
add_subdirectory(boot_test)

log_info("TestFramework" "All test modules configured")
