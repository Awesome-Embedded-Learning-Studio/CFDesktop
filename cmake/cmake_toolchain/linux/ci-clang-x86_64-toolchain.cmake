# =============================================================================
# CI Build Toolchain Configuration (Clang x86_64/AMD64)
# =============================================================================
# Clang toolchain for CI Docker x86_64 builds
#
# Qt installation path: /opt/Qt/6.8.1/gcc_64 (same as GCC, compatible)
#
# Usage:
#   cmake -DUSE_TOOLCHAIN=linux/ci-clang-x86_64 -S . -B build
# =============================================================================

set(CMAKE_SYSTEM_NAME Linux)

# -----------------------------------------------------------------------------
# Compiler configuration
# Ubuntu 24.04 ships Clang 18 with full C++23 support
# -----------------------------------------------------------------------------
set(CMAKE_C_COMPILER "clang" CACHE FILEPATH "C compiler")
set(CMAKE_CXX_COMPILER "clang++" CACHE FILEPATH "C++ compiler")

# -----------------------------------------------------------------------------
# Qt6 search path (same as GCC CI toolchain)
# Qt pre-built binaries from aqtinstall are compiler-agnostic on Linux
# -----------------------------------------------------------------------------
set(QT6_BASE_DIR "/opt/Qt/6.8.1/gcc_64")
set(QT6_CMAKE_DIR "${QT6_BASE_DIR}/lib/cmake/Qt6")

if(DEFINED ENV{Qt6_DIR})
    set(QT6_CMAKE_DIR "$ENV{Qt6_DIR}")
    get_filename_component(QT6_BASE_DIR "${QT6_CMAKE_DIR}/../.." ABSOLUTE)
elseif(DEFINED ENV{QT6_DIR})
    set(QT6_BASE_DIR "$ENV{QT6_DIR}")
    set(QT6_CMAKE_DIR "${QT6_BASE_DIR}/lib/cmake/Qt6")
endif()

set(Qt6_DIR "${QT6_CMAKE_DIR}" CACHE PATH "Qt6 installation directory")
set(CMAKE_PREFIX_PATH "${QT6_BASE_DIR}")
list(APPEND CMAKE_PREFIX_PATH "${QT6_BASE_DIR}")

# -----------------------------------------------------------------------------
# ccache acceleration (available in Docker container)
# -----------------------------------------------------------------------------
find_program(CCACHE_PROGRAM ccache CACHE)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}" CACHE FILEPATH "C compiler launcher")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}" CACHE FILEPATH "CXX compiler launcher")
endif()
