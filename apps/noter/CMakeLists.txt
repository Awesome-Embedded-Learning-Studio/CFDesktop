# Noter standalone application.
# Ported from CCIMXDesktop's CCIMXNoter; UI uses QuarkWidgets MD3 widgets.
# CFDesktop launches this binary via AppLaunchService (QProcess), so it runs in
# its own process (isolated from the desktop shell).

# Standalone Noter executable (consumed by CFDesktop via QProcess launch).
qt_add_executable(noter
    main.cpp
    noter_panel.cpp
)

target_link_libraries(noter PRIVATE
    QuarkWidgets::quarkwidgets
    Qt6::Widgets
)

# App package is self-contained under <bin>/../apps/noter/ (executable +
# manifest in the same dir). AppDiscoverer scans <bin>/../apps/<id>/app.json.
set(NOTER_APP_DIR "${CMAKE_BINARY_DIR}/apps/noter")
set_target_properties(noter PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${NOTER_APP_DIR}"
)

# Deploy the manifest next to the executable (at configure time).
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/app.json" "${NOTER_APP_DIR}/app.json" COPYONLY)
