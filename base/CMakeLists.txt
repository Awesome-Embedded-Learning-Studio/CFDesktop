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

# Include subdirectory CMakeLists (static libraries)
add_subdirectory(system)
add_subdirectory(device)

# ============================================================
# Unified Base Library - Single DLL: cfbase.dll
# ============================================================

# Create the unified shared library
add_library(cfbase SHARED)

# Link static libraries from sub-modules with --whole-archive to ensure
# all symbols (including dllexport) are included in cfbase.dll.
# Without this, the linker skips static lib objects since cfbase has no own sources.
# Force all objects into cfbase.dll (PRIVATE: only affects cfbase's link step)
target_link_libraries(cfbase PRIVATE
    -Wl,--whole-archive
    cfbase_cpu
    cfbase_memory
    cfbase_network
    cfbase_gpu
    cfbase_console
    -Wl,--no-whole-archive
)

# Set include directories
# Expose both include/ (for public headers) and . (for sub-module headers like device/console)
target_include_directories(cfbase PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
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
add_library(CFDesktop::base_console ALIAS cfbase)
