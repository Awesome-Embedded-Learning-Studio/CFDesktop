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
if(WIN32)
    target_compile_definitions(CFDesktop_shared PRIVATE CFDESKTOP_EXPORTS)
endif()

# Set symbol visibility to hidden on Unix/Linux
if(UNIX AND NOT APPLE)
    target_compile_options(CFDesktop_shared PRIVATE -fvisibility=hidden)
endif()

# Include directories
target_include_directories(CFDesktop_shared PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/base
    ${CMAKE_CURRENT_SOURCE_DIR}/main
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
# Main executable (links against the shared library)
# ============================================================
add_executable(CFDesktop main.cpp desktop_entry.cpp)

# Link against the unified CFDesktop shared library
# Also link CFDesktopEarlySession directly for symbols not exported from DLL
target_link_libraries(CFDesktop PRIVATE
    CFDesktop_shared CFDesktopMain cflogger cfbase
)

log_info("Desktop Buildings" "Configuring Desktop Buildings Done")
