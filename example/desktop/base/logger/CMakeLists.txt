# ============================================================
# Logger Examples
# ============================================================

# Logger Usage Example
add_executable(example_logger_usage example_usage.cpp logger_init.cpp)
target_link_libraries(example_logger_usage PRIVATE CFDesktop::logger)
if(WIN32)
    target_compile_definitions(example_logger_usage PRIVATE CFLOG_STATIC_BUILD)
endif()
cf_set_example_output_dir(example_logger_usage "desktop/base")
cf_register_example_launcher(example_logger_usage "desktop/base")