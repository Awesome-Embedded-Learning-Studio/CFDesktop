# Base library - fundamental utilities and platform abstractions
cmake_minimum_required(VERSION 3.16)

# ============================================================
# Header-only library for base/include (utilities, hash, expected, etc.)
# ============================================================
add_library(cfbase_headers INTERFACE)
target_include_directories(cfbase_headers INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_library(CFDesktop::base_headers ALIAS cfbase_headers)

# Include subdirectory CMakeLists (OBJECT libraries)
add_subdirectory(system/cpu)
add_subdirectory(system/memory)

# ============================================================
# Unified Base Library - Single DLL: cfbase.dll
# ============================================================

# Create the unified shared library
add_library(cfbase SHARED)

# Merge all object files from sub-modules
target_sources(cfbase PRIVATE
    $<TARGET_OBJECTS:cfbase_cpu>
    $<TARGET_OBJECTS:cfbase_memory>
)

# Set include directories
target_include_directories(cfbase PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link dependencies
target_link_libraries(cfbase PUBLIC
    Qt6::Core
)

# Platform-specific link libraries
if(WIN32)
    target_link_libraries(cfbase PUBLIC
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
# For backward compatibility
add_library(CFDesktop::base_cpu ALIAS cfbase)
add_library(CFDesktop::base_memory ALIAS cfbase)
