# ============================================================
# ConfigManager Examples
# ============================================================

# ConfigManager Usage Example
add_executable(example_config_manager_usage
    example_usage.cpp
    logger_init.cpp
)
target_link_libraries(example_config_manager_usage PRIVATE
    CFDesktop::config
    CFDesktop::logger
    Qt6::Core
)
if(WIN32)
    target_compile_definitions(example_config_manager_usage PRIVATE CFLOG_STATIC_BUILD)
endif()
cf_set_example_output_dir(example_config_manager_usage "desktop/base")
cf_register_example_launcher(example_config_manager_usage "desktop/base")
