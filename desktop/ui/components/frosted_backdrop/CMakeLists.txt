# Frosted-glass (acrylic) backdrop renderer for shell panels.
# Pure Qt Gui utility: crop -> box blur -> tint -> acrylic grain. Depends on
# nothing in desktop/ (three-layer clean) so it stays unit-testable in isolation.
add_library(cfdesktop_frosted_backdrop STATIC
    frosted_backdrop.cpp
)

target_include_directories(cfdesktop_frosted_backdrop PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

target_link_libraries(cfdesktop_frosted_backdrop PUBLIC Qt6::Gui)
