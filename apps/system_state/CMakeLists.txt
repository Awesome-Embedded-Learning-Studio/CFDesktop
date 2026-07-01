# System State standalone application.
# Ported from CCIMXDesktop's SystemState. Unlike the original (which shipped
# its own platform-specific CPU/memory fetchers), this version reuses the
# mature base/ probes (cfbase) for system data — showcasing the base-layer
# capability. The UI is rebuilt with QuarkWidgets MD3 widgets + plain Qt
# labels (no QtCharts dependency, no .ui files).
# CFDesktop launches this binary via AppLaunchService (QProcess), so it runs
# in its own process (isolated from the desktop shell).

# Standalone System State executable (consumed by CFDesktop via QProcess).
qt_add_executable(system_state
    main.cpp
    system_state_panel.cpp
)

target_link_libraries(system_state PRIVATE
    cfbase
    QuarkWidgets::quarkwidgets
    Qt6::Widgets
)

# App package is self-contained under <bin>/../apps/system_state/ (executable +
# manifest in the same dir). AppDiscoverer scans <bin>/../apps/<id>/app.json,
# resolves exec to the co-located absolute path, and AppLaunchService launches
# it directly — no working-directory dependency, no PATH search needed.
set(SYSTEM_STATE_APP_DIR "${CMAKE_BINARY_DIR}/apps/system_state")
set_target_properties(system_state PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${SYSTEM_STATE_APP_DIR}"
)

# Deploy the manifest next to the executable (at configure time).
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/app.json" "${SYSTEM_STATE_APP_DIR}/app.json" COPYONLY)
