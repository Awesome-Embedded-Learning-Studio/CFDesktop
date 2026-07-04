# Platform Strategy Library
# Provides platform-specific desktop property strategies

# Platform-specific sources
set(PLATFORM_SOURCES
    platform_helper.cpp
    DesktopPropertyStrategyFactory.cpp
    IDesktopPropertyStrategy.cpp
    IDesktopDisplaySizeStrategy.cpp
    display_backend_helper.cpp
    shell_layer_helper.cpp
)

set(PLATFORM_HEADERS
    platform_helper.h
    DesktopPropertyStrategyFactory.h
    IDesktopPropertyStrategy.h
    display_backend_helper.h
    shell_layer_helper.h
)

# Add platform-specific subdirectories.
# CFDESKTOP_EMBEDDED selects the DirectRender backend (Qt linuxfb / EGLFS) for
# targets without a windowing system (e.g. i.MX6ULL). It is mutually exclusive
# with the linux_wsl X11 backend and the Windows backend.
option(CFDESKTOP_EMBEDDED "Build the embedded DirectRender platform backend (linuxfb/EGLFS, no X11/compositor)" OFF)

if(CFDESKTOP_EMBEDDED)
    # Embedded DirectRender platform -- no X11, no compositor.
    file(GLOB embedded_sources "${CMAKE_CURRENT_SOURCE_DIR}/embedded/*.cpp")
    list(APPEND PLATFORM_SOURCES ${embedded_sources})
elseif(UNIX AND NOT APPLE)
    # Linux/WSL platform
    if(IS_WSL OR EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/linux_wsl")
        file(GLOB linux_wsl_sources "${CMAKE_CURRENT_SOURCE_DIR}/linux_wsl/*.cpp")
        list(APPEND PLATFORM_SOURCES ${linux_wsl_sources})

        # XCB dependency for X11 window tracking on WSL
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(XCB IMPORTED_TARGET xcb)
    endif()
elseif(WIN32)
    # Windows platform
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/windows")
        file(GLOB windows_sources "${CMAKE_CURRENT_SOURCE_DIR}/windows/*.cpp")
        list(APPEND PLATFORM_SOURCES ${windows_sources})
    endif()
endif()

# Create static library for platform strategy
add_library(cf_desktop_ui_platform STATIC
    ${PLATFORM_SOURCES}
)

# Export symbols for Windows DLL - define when building static library that will be linked into DLL
if(WIN32)
    target_compile_definitions(cf_desktop_ui_platform PRIVATE CFDESKTOP_EXPORTS)
endif()

# Include current directory and platform-specific subdirectories
target_include_directories(cf_desktop_ui_platform PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# Add platform-specific subdirectory includes
if(CFDESKTOP_EMBEDDED)
    target_include_directories(cf_desktop_ui_platform PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/embedded>
    )
elseif(UNIX AND NOT APPLE)
    if(IS_WSL OR EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/linux_wsl")
        target_include_directories(cf_desktop_ui_platform PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/linux_wsl>
        )
    endif()
elseif(WIN32)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/windows")
        target_include_directories(cf_desktop_ui_platform PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/windows>
        )
    endif()
endif()

target_link_libraries(cf_desktop_ui_platform
PUBLIC
    cfbase
    cflogger
    cf_desktop_ui_base
    cf_desktop_components
PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    QuarkWidgets::quarkwidgets            # For qt_format.h (std::formatter<QString>)
)

if(CFDESKTOP_EMBEDDED)
    log_info("UI Backend" "Using Embedded DirectRender Backend (linuxfb/EGLFS).")
elseif(UNIX AND NOT APPLE)
    if(XCB_FOUND)
        log_info("UI Backend" "Using XCB Backend as the Desktop UI Render Backend.")
        target_link_libraries(cf_desktop_ui_platform PRIVATE PkgConfig::XCB)
        target_compile_definitions(cf_desktop_ui_platform PRIVATE CFDESKTOP_HAS_XCB)
    endif()
endif()
