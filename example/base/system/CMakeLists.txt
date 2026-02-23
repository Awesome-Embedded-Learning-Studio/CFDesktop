# CPU Info
add_executable(example_cpu_info example_cpu_info.cpp)
target_link_libraries(example_cpu_info PRIVATE cfbase_cpu)

# Memory Info
add_executable(example_memory_info example_memory_info.cpp)
target_link_libraries(example_memory_info PRIVATE cfbase_memory)
