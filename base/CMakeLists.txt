# Base library - fundamental utilities and platform abstractions
cmake_minimum_required(VERSION 3.16)

# Create interface library for headers-only components
add_library(cfbase_interface INTERFACE)

# Target for CPU info module
add_library(cfbase_cpu STATIC
    system/cpu/cfcpu.cpp
    system/cpu/cfcpu_profile.cpp
)

target_include_directories(cfbase_cpu
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/system
)

target_link_libraries(cfbase_cpu
    PUBLIC
        Qt6::Core
)

# Platform-specific sources
if(WIN32)
    # Windows specific implementations
    target_sources(cfbase_cpu PRIVATE
        system/cpu/private/win_impl/cpu.cpp
    )
    target_include_directories(cfbase_cpu PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/base/windows
    )
    # Link Windows WMI libraries for CPU info
    target_link_libraries(cfbase_cpu PUBLIC wbemuuid PowrProf pdh)
elseif(UNIX)
    # Linux/Unix specific implementations will be added here
    # target_sources(cfbase_cpu PRIVATE system/implements/linux_impl/...)
endif()

# Alias for convenience
add_library(CFDesktop::base_cpu ALIAS cfbase_cpu)
add_library(CFDesktop::base_interface ALIAS cfbase_interface)

# Group sources for IDEs
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "base" FILES
    system/cpu/cfcpu.cpp
    system/cpu/private/win_impl/cpu.cpp
    base/windows/co_helper.cpp
)
