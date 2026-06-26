# Window placement policy: clamps external windows into the desktop work area
# (central rect between top status bar and bottom taskbar). Pure logic over
# IWindow + QRect — no platform branches, no preprocessor conditionals — so it
# stays unit-testable without any backend or display.
add_library(cfdesktop_window_placement STATIC
    window_placement_policy.cpp
)

target_include_directories(cfdesktop_window_placement PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

target_link_libraries(cfdesktop_window_placement
PUBLIC
    Qt6::Core
PRIVATE
    cfbase  # aex headers pulled in transitively by IWindow.h
)
