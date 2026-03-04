# ============================================================
# Base Examples - System Info
# ============================================================

# CPU Info
add_executable(example_cpu_info example_cpu_info.cpp)
target_link_libraries(example_cpu_info PRIVATE cfbase)
cf_set_example_output_dir(example_cpu_info "base")
cf_register_example_launcher(example_cpu_info "base")

# Memory Info
add_executable(example_memory_info example_memory_info.cpp)
target_link_libraries(example_memory_info PRIVATE cfbase)
cf_set_example_output_dir(example_memory_info "base")
cf_register_example_launcher(example_memory_info "base")
