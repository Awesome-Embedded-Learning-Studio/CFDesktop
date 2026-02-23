# Base library - fundamental utilities and platform abstractions
cmake_minimum_required(VERSION 3.16)

# Include subdirectory CMakeLists
add_subdirectory(system/cpu)
add_subdirectory(system/memory)

# ============================================================
# Unified Base Library
# ============================================================

# Create interface library that merges all sub-libraries
add_library(cfbase INTERFACE)

# Link all CPU and memory modules
target_link_libraries(cfbase INTERFACE
    cfbase_cpu_info
    cfbase_cpu_profile
    cfbase_cpu_bonus
    cfbase_cpu_features
    cfbase_memory_info
    Qt6::Core
)

# Platform-specific link for unified library
if(WIN32)
    target_link_libraries(cfbase INTERFACE
        wbemuuid
        PowrProf
        pdh
        psapi
    )
endif()

# ============================================================
# Aliases
# ============================================================
add_library(CFDesktop::base ALIAS cfbase)
