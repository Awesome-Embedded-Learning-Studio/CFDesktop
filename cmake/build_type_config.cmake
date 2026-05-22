# ------ Build Type Configuration ------
# Validate and set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (Debug Release RelWithDebInfo)" FORCE)
    message(STATUS "CMAKE_BUILD_TYPE not set, defaulting to: ${CMAKE_BUILD_TYPE}")
endif()

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo")

# Set compiler flags for each build type (use CACHE FORCE to override CMake defaults)
if(MSVC)
    set(_CF_DEBUG_CXX_FLAGS "/Od /Zi /D_CFDESKTOPDEBUG")
    set(_CF_RELEASE_CXX_FLAGS "/O2 /DNDEBUG")
    set(_CF_RELWITHDEBINFO_CXX_FLAGS "/O2 /Zi /DNDEBUG")
    set(_CF_RELEASE_LINKER_FLAGS "")
else()
    set(_CF_DEBUG_CXX_FLAGS "-O0 -g -D_CFDESKTOPDEBUG")
    set(_CF_RELEASE_CXX_FLAGS "-O3 -DNDEBUG")
    set(_CF_RELWITHDEBINFO_CXX_FLAGS "-O2 -g -DNDEBUG")
    set(_CF_RELEASE_LINKER_FLAGS "-s")
endif()

# Debug: No optimization, full debug info
set(CMAKE_CXX_FLAGS_DEBUG "${_CF_DEBUG_CXX_FLAGS}" CACHE STRING "Flags used by the C++ compiler during Debug builds" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "" CACHE STRING "Linker flags used during Debug builds" FORCE)
# For Static Library Relocatable
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# Release: Maximum optimization, no debug info
set(CMAKE_CXX_FLAGS_RELEASE "${_CF_RELEASE_CXX_FLAGS}" CACHE STRING "Flags used by the C++ compiler during Release builds" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${_CF_RELEASE_LINKER_FLAGS}" CACHE STRING "Linker flags used during Release builds" FORCE)

# RelWithDebInfo: Optimization (-O2) + full debug info
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${_CF_RELWITHDEBINFO_CXX_FLAGS}" CACHE STRING "Flags used by the C++ compiler during RelWithDebInfo builds" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "" CACHE STRING "Linker flags used during RelWithDebInfo builds" FORCE)

string(TOUPPER "${CMAKE_BUILD_TYPE}" _build_type_upper)

# Variables for build type-specific flags (used by summary table)
set(_cxx_flags_var   "CMAKE_CXX_FLAGS_${_build_type_upper}")
set(_link_flags_var  "CMAKE_EXE_LINKER_FLAGS_${_build_type_upper}")
