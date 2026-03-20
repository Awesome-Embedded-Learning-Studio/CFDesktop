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
add_subdirectory(system/cpu)
add_subdirectory(system/memory)
add_subdirectory(device)

# ============================================================
# Unified Base Library - Single DLL: cfbase.dll
# ============================================================

# Create the unified static library
add_library(cfbase STATIC)

# Link static libraries from sub-modules
target_link_libraries(cfbase PUBLIC
    cfbase_cpu
    cfbase_memory
    cfbase_console
)

# Set include directories
# Expose both include/ (for public headers) and . (for sub-module headers like device/console)
target_include_directories(cfbase PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
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
