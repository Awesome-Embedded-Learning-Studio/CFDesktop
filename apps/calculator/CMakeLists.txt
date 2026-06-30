# Calculator standalone application.
# Ported from CCIMXDesktop's Caculator; UI uses cfui MD3 widgets.
# CFDesktop launches this binary via AppLaunchService (QProcess), so it runs in
# its own process (isolated from the desktop shell).

# Expression parser library (QString-based; lives outside base/ because base/
# must not depend on Qt).
add_library(cfdesktop_calculator_parser STATIC
    core/Parser.cpp
    core/NumberNode.cpp
    core/BinaryOpTreeNode.cpp
    core/UnaryOpTreeNode.cpp
    core/FunctorTreeNode.cpp
    core/ExpressionEvaluator.cpp
)

target_include_directories(cfdesktop_calculator_parser PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core>
)

target_link_libraries(cfdesktop_calculator_parser PUBLIC Qt6::Core)

# Standalone Calculator executable (consumed by CFDesktop via QProcess launch).
qt_add_executable(calculator
    main.cpp
    calculator_panel.cpp
)

target_link_libraries(calculator PRIVATE
    cfdesktop_calculator_parser
    QuarkWidgets::quarkwidgets
    Qt6::Widgets
)

# App package is self-contained under <bin>/../apps/calculator/ (executable +
# manifest in the same dir). AppDiscoverer scans <bin>/../apps/<id>/app.json,
# resolves exec to the co-located absolute path, and AppLaunchService launches
# it directly — no working-directory dependency, no PATH search needed.
set(CALC_APP_DIR "${CMAKE_BINARY_DIR}/apps/calculator")
set_target_properties(calculator PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CALC_APP_DIR}"
)

# Deploy the manifest next to the executable (at configure time).
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/app.json" "${CALC_APP_DIR}/app.json" COPYONLY)
