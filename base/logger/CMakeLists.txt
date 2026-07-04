# Logger module
# Shared library — provides both headers and implementation
add_library(cflogger SHARED)

# Sources
target_sources(cflogger PRIVATE
    src/logger/cflog.cpp
    src/impl/cflog_impl.cpp
    src/async_queue/async_queue.cpp
    src/formatter/console_formatter.cpp
    src/formatter/file_formatter.cpp
    src/sinks/console_sink.cpp
    src/sinks/file_sink.cpp
)

# Include directories
target_include_directories(cflogger PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_include_directories(cflogger PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link to base library for access to base headers and system utilities
target_link_libraries(cflogger PUBLIC
    CFDesktop::base
)

# Alias for consistent naming
add_library(CFDesktop::logger ALIAS cflogger)

