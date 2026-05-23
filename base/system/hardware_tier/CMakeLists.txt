# Hardware Tier Assessment Module
cmake_minimum_required(VERSION 3.16)

add_library(cfbase_hardware_tier STATIC)
set_target_properties(cfbase_hardware_tier PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

target_compile_definitions(cfbase_hardware_tier PRIVATE CFBASE_EXPORTS)

target_include_directories(cfbase_hardware_tier
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_sources(cfbase_hardware_tier PRIVATE
    hardware_tier.cpp
    default/default_collector.cpp
    default/default_cpu_scorer.cpp
    default/default_gpu_scorer.cpp
    default/default_memory_scorer.cpp
    default/default_display_scorer.cpp
    default/default_assessor.cpp
    default/default_policy.cpp
)

target_link_libraries(cfbase_hardware_tier PRIVATE
    cfbase_cpu
    cfbase_gpu
    cfbase_memory
)
