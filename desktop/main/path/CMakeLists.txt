# Making Components with early sessions

add_library(CFDesktopPathSettings STATIC)

target_sources(CFDesktopPathSettings PRIVATE 
    desktop_main_path_resolvers.cpp
    desktop_self_path_resolvers.cpp)

target_include_directories(CFDesktopPathSettings PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(CFDesktopPathSettings PUBLIC cflogger_headers Qt6::Core)

if(WIN32)
    target_compile_definitions(CFDesktopPathSettings PRIVATE CFLOG_STATIC_BUILD)
endif()
