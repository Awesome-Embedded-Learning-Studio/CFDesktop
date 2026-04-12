# Path module — desktop path resolution utilities
add_library(cfpath STATIC)

target_sources(cfpath PRIVATE
    src/desktop_main_path_resolvers.cpp
    src/desktop_self_path_resolvers.cpp
)

target_include_directories(cfpath PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(cfpath PUBLIC
    CFDesktop::base
    cflogger
    Qt6::Core
)

add_library(CFDesktop::path ALIAS cfpath)
