# =============================================================================
# CI Build Toolchain Configuration (Windows MSVC)
# =============================================================================
# MSVC toolchain for Windows CI on github-actions runners
#
# Qt installation path: C:/Qt/6.8.1/msvc2022_64
# Compiler: auto-detected by Visual Studio generator (MSVC v143)
#
# Usage:
#   cmake -DUSE_TOOLCHAIN=windows/ci-msvc -S . -B build
# =============================================================================

set(CMAKE_SYSTEM_NAME Windows)

# -----------------------------------------------------------------------------
# Qt6 search path (Windows CI runner with aqtinstall)
# -----------------------------------------------------------------------------
set(QT6_BASE_DIR "C:/Qt/6.8.1/msvc2022_64")
set(QT6_CMAKE_DIR "${QT6_BASE_DIR}/lib/cmake/Qt6")

# Allow environment variable overrides for local testing
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
# ccache acceleration
# GitHub Actions installs ccache before configuring this toolchain.
# -----------------------------------------------------------------------------
find_program(CCACHE_PROGRAM ccache CACHE)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}" CACHE FILEPATH "C compiler launcher")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}" CACHE FILEPATH "CXX compiler launcher")
endif()
