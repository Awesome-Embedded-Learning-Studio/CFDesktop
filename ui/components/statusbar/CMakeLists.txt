# Status bar implementation (QWidget-based top-edge panel).
add_library(cfdesktop_statusbar STATIC
    status_bar.cpp
    statusbar_icons.qrc
)

target_include_directories(cfdesktop_statusbar PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
)

target_link_libraries(
    cfdesktop_statusbar
PUBLIC
    QuarkWidgets::quarkwidgets           # ThemeManager, color/typography tokens, ICFTheme
PRIVATE
    Qt6::Widgets
    cfbase         # WeakPtr / WeakPtrFactory
    cflogger       # Diagnostic logging for icon-mask loading
    cfdesktop_frosted_backdrop  # Frosted-glass backdrop renderer
    cfdesktop_notification       # Unread badge: NotificationService::count()
)
