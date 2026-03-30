log_info("Desktop Buildings" "Start Configuring Desktop Buildings")

# ============================================================
# Add static library modules first
# ============================================================
add_subdirectory(base)
add_subdirectory(ui)
add_subdirectory(main)
# ============================================================
# Unified CFDesktop Shared Library
# ============================================================
# Combines all static libraries into a single shared library libCFDesktop.so

log_info("Desktop" "Creating unified CFDesktop shared library")

add_library(CFDesktop_shared SHARED)

# Export symbols for Windows DLL
# CFLOG_STATIC_BUILD: cflogger symbols resolve as regular refs within the DLL
# CFDESKTOP_STATIC_BUILD: NOT set here — desktop_run_session.cpp needs
#   CF_DESKTOP_EXPORT = dllexport (CFDESKTOP_EXPORTS is defined on this target)
if(WIN32)
    target_compile_definitions(CFDesktop_shared PRIVATE
        CFDESKTOP_EXPORTS
        CFLOG_STATIC_BUILD
    )
endif()

# Set symbol visibility to hidden on Unix/Linux
if(UNIX AND NOT APPLE)
    target_compile_options(CFDesktop_shared PRIVATE -fvisibility=hidden)
endif()

# Include directories
target_include_directories(CFDesktop_shared PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/base>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/main>
)

# Collect all static libraries to be linked into the shared library
set(CFDESKTOP_STATIC_LIBS
    cfbase
    cfui
    cflogger
    cffilesystem
    cfconfig
    cfasciiart
    CFDesktopMain
    CFDesktopUi
    cf_desktop_render
)

# Sources compiled directly into the DLL (with CFDESKTOP_EXPORTS defined)
target_sources(CFDesktop_shared PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/desktop_run_session.cpp
)

# Link all static libraries using --whole-archive
# This ensures all symbols are included even if not directly referenced
# Supported on both Unix and MinGW/Windows
if(UNIX AND NOT APPLE)
    target_link_libraries(CFDesktop_shared PRIVATE
        "-Wl,--whole-archive"
        ${CFDESKTOP_STATIC_LIBS}
        "-Wl,--no-whole-archive"
    )
elseif(WIN32)
    # MinGW supports --whole-archive on Windows too - required for DLL symbol export
    target_link_libraries(CFDesktop_shared PRIVATE
        "-Wl,--whole-archive"
        ${CFDESKTOP_STATIC_LIBS}
        "-Wl,--no-whole-archive"
    )
else()
    target_link_libraries(CFDesktop_shared PRIVATE
        ${CFDESKTOP_STATIC_LIBS}
    )
endif()

# Link Qt libraries
target_link_libraries(CFDesktop_shared PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Platform-specific link libraries (from base)
if(WIN32)
    target_link_libraries(CFDesktop_shared PUBLIC
        wbemuuid
        PowrProf
        pdh
        psapi
    )
endif()

# Add alias for consistent naming
add_library(CFDesktop::lib ALIAS CFDesktop_shared)
# ============================================================
# Main executable — thin shell that calls into the DLL
# ============================================================
add_executable(CFDesktop main.cpp)

# The EXE only links the DLL import library.
# All code (CFDesktopMain, desktop_entry, init stages, etc.) lives in the DLL.
target_link_libraries(CFDesktop PRIVATE
    CFDesktop_shared
)

log_info("Desktop Buildings" "Configuring Desktop Buildings Done")
