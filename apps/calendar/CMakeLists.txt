# Calendar standalone application.
# Ported from CCIMXDesktop's CCCalendar; UI uses QuarkWidgets MD3 widgets plus
# Qt's QCalendarWidget for the month grid (MVP — a self-drawn MD3 calendar may
# replace it later).
# CFDesktop launches this binary via AppLaunchService (QProcess), so it runs in
# its own process (isolated from the desktop shell).

# Standalone Calendar executable (consumed by CFDesktop via QProcess launch).
qt_add_executable(calendar
    main.cpp
    calendar_panel.cpp
)

target_link_libraries(calendar PRIVATE
    QuarkWidgets::quarkwidgets
    Qt6::Widgets
)

# App package is self-contained under <bin>/../apps/calendar/ (executable +
# manifest in the same dir). AppDiscoverer scans <bin>/../apps/<id>/app.json.
set(CALENDAR_APP_DIR "${CMAKE_BINARY_DIR}/apps/calendar")
set_target_properties(calendar PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CALENDAR_APP_DIR}"
)

# Deploy the manifest next to the executable (at configure time).
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/app.json" "${CALENDAR_APP_DIR}/app.json" COPYONLY)
