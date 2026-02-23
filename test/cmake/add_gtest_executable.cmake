# =============================================================================
# Helper function to add GoogleTest executable with CTest registration
# =============================================================================
#
# Usage:
#   add_gtest_executable(
#       TEST_NAME      <test_name>
#       SOURCE_FILE    <source_file.cpp>
#       LINK_LIBRARIES <lib1> [<lib2> ...]
#       LABELS         <label1> [<label2> ...]
#       LOG_MODULE     <module_name_for_logging>
#   )
#
# Parameters:
#   TEST_NAME      - Name of the test executable (without extension)
#   SOURCE_FILE    - Source file name (can be with or without .cpp extension)
#   LINK_LIBRARIES - List of libraries to link against
#   LABELS         - Semicolon-separated list of test labels for CTest
#   LOG_MODULE     - Module name for logging configuration
#
# Example:
#   add_gtest_executable(
#       TEST_NAME      test_cpu_info_query
#       SOURCE_FILE    test_cpu_info_query.cpp
#       LINK_LIBRARIES cfbase_cpu;GTest::gtest;GTest::gtest_main
#       LABELS         "base;system"
#       LOG_MODULE     base_system
#   )
#
# =============================================================================
function(add_gtest_executable)
    # Parse arguments
    cmake_parse_arguments(
        ARG
        ""                  # No options
        "TEST_NAME;SOURCE_FILE;LOG_MODULE"  # Single-value arguments
        "LINK_LIBRARIES;LABELS"             # Multi-value arguments
        ${ARGN}
    )

    # Validate required arguments
    if(NOT ARG_TEST_NAME)
        message(FATAL_ERROR "add_gtest_executable: TEST_NAME is required")
    endif()
    if(NOT ARG_SOURCE_FILE)
        message(FATAL_ERROR "add_gtest_executable: SOURCE_FILE is required")
    endif()
    if(NOT ARG_LINK_LIBRARIES)
        message(FATAL_ERROR "add_gtest_executable: LINK_LIBRARIES is required")
    endif()
    if(NOT ARG_LABELS)
        message(FATAL_ERROR "add_gtest_executable: LABELS is required")
    endif()
    if(NOT ARG_LOG_MODULE)
        message(FATAL_ERROR "add_gtest_executable: LOG_MODULE is required")
    endif()

    # Create executable
    add_executable(${ARG_TEST_NAME}
        ${ARG_SOURCE_FILE}
    )

    # Link libraries
    target_link_libraries(${ARG_TEST_NAME}
        PRIVATE ${ARG_LINK_LIBRARIES}
    )

    # Register with CTest
    add_test(NAME ${ARG_TEST_NAME} COMMAND ${ARG_TEST_NAME})

    # Set test properties
    set_tests_properties(${ARG_TEST_NAME} PROPERTIES
        LABELS "${ARG_LABELS}"
    )

    # Log configuration
    log_info("${ARG_LOG_MODULE}" "  - ${ARG_TEST_NAME}")
endfunction()
