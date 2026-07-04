log_info("Desktop Init Module" "Start Configuring Desktop Init")

add_library(CFDesktopInit STATIC)

target_sources(CFDesktopInit PRIVATE
    init_early_session.cpp
    init_session_chain.cpp
    init_session.cpp
    init_settings.cpp
    gui_progress/gui_init_stage.cpp
    early_gain/early_pass_stage.cpp
    desktop_backbone_init/desktop_backbone_init.cpp
)

target_include_directories(CFDesktopInit
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(CFDesktopInit PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    CFDesktopEarlySession
    cf_desktop_ui_widget_init_session  # Link to UI widget library (formerly gui_progress)
)

log_info("Desktop Init Module" "Start Configuring Desktop Finish")