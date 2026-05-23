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

# Network Info
add_executable(example_network_info example_network_info.cpp)
target_link_libraries(example_network_info PRIVATE cfbase)
cf_set_example_output_dir(example_network_info "base")
cf_register_example_launcher(example_network_info "base")

# GPU Info
add_executable(example_gpu_info example_gpu_info.cpp)
target_link_libraries(example_gpu_info PRIVATE cfbase)
cf_set_example_output_dir(example_gpu_info "base")
cf_register_example_launcher(example_gpu_info "base")

# Hardware Tier Assessment
add_executable(example_hardware_tier example_hardware_tier.cpp)
target_link_libraries(example_hardware_tier PRIVATE cfbase)
cf_set_example_output_dir(example_hardware_tier "base")
cf_register_example_launcher(example_hardware_tier "base")
