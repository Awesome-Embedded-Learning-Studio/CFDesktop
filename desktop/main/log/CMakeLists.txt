# Making Components with early sessions

add_library(CFDesktopLog STATIC)

target_sources(CFDesktopLog PRIVATE log_helper.cpp)

target_include_directories(CFDesktopLog PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(CFDesktopLog PUBLIC cflogger_headers Qt6::Core)

if(WIN32)
    target_compile_definitions(CFDesktopLog PRIVATE CFLOG_STATIC_BUILD)
endif()
