log_info("Desktop Main" "Creating unified CFDesktopMain library")

# Collect source variables from subdirectories
add_subdirectory(early_session/impl)
# CFDesktopEarlySession sources (merged from early_session/CMakeLists.txt)
set(EARLY_SESSION_SOURCES
    early_session/init_chain/init_chain.cpp
    early_session/settings/early_settings.cpp
    early_session/early_handle/early_handle.cpp
    ${IMPL_SOURCES}
)

# CFDesktopInit sources (merged from init/CMakeLists.txt)
set(INIT_SOURCES
    init/init_early_session.cpp
    init/init_session_chain.cpp
    init/init_session.cpp
    init/init_settings.cpp
    init/gui_progress/gui_init_stage.cpp
    init/early_gain/early_pass_stage.cpp
    init/desktop_backbone_init/desktop_backbone_init.cpp
    init/desktop_config_init/desktop_config_init.cpp
)

add_library(CFDesktopMain STATIC)

target_sources(CFDesktopMain PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/desktop_entry.cpp
    ${EARLY_SESSION_SOURCES}
    ${INIT_SOURCES}
)

# Include directories (merged from CFDesktopEarlySession and CFDesktopInit)
target_include_directories(CFDesktopMain PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/early_session>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/init>
    $<BUILD_INTERFACE:${CFDESKTOP_SETTINGS_OUTPUT_DIR}>
)

target_link_libraries(CFDesktopMain PUBLIC
    cfbase cflogger cfasciiart cffilesystem cffundamental cfpath cfconfig
    cf_desktop_ui_widget_init_session
    Qt6::Core Qt6::Gui Qt6::Widgets
)
# CFDesktopMain lives entirely inside CFDesktop_shared.dll (via --whole-archive).
# CFDESKTOP_STATIC_BUILD must be set so that CF_DESKTOP_EXPORT expands to
# nothing — code inside the DLL must use regular (non-dllimport) references
# to its own symbols.
if(WIN32)
    target_compile_definitions(CFDesktopMain PRIVATE
        CFDESKTOP_STATIC_BUILD
    )
endif()

log_info("Desktop Main" "CFDesktopMain library configured")
