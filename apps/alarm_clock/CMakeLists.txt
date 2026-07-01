# Alarm Clock standalone application.
# Ported from CCIMXDesktop's AlarmyClock; UI uses QuarkWidgets MD3 widgets.
# CFDesktop launches this binary via AppLaunchService (QProcess), so it runs in
# its own process (isolated from the desktop shell).

# Standalone Alarm Clock executable (consumed by CFDesktop via QProcess launch).
qt_add_executable(alarm_clock
    main.cpp
    alarm_clock_panel.cpp
)

target_link_libraries(alarm_clock PRIVATE
    QuarkWidgets::quarkwidgets
    Qt6::Widgets
)

# App package is self-contained under <bin>/../apps/alarm_clock/ (executable +
# manifest in the same dir). AppDiscoverer scans <bin>/../apps/<id>/app.json,
# resolves exec to the co-located absolute path, and AppLaunchService launches
# it directly — no working-directory dependency, no PATH search needed.
set(ALARM_CLOCK_APP_DIR "${CMAKE_BINARY_DIR}/apps/alarm_clock")
set_target_properties(alarm_clock PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${ALARM_CLOCK_APP_DIR}"
)

# Deploy the manifest next to the executable (at configure time).
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/app.json" "${ALARM_CLOCK_APP_DIR}/app.json" COPYONLY)
