# Logger module
# Static library (has .cpp sources)
add_library(cflogger STATIC)

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

# =============================================================================
# cflogger_headers — Header-only INTERFACE library
# =============================================================================
# DLL-internal modules that only call the public API (cflog.h free functions)
# should link this instead of the static lib, to avoid duplicating compiled
# symbols. Only CFDesktop_shared (via --whole-archive) and CFDesktopMain
# (PRIVATE, for internal classes like FileSink/ConsoleSink) link the real lib.
add_library(cflogger_headers INTERFACE)
target_include_directories(cflogger_headers INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(cflogger_headers INTERFACE CFDesktop::base)
add_library(CFDesktop::logger_headers ALIAS cflogger_headers)

# cflogger itself: mark symbols for DLL export when building on Windows
if(WIN32)
    target_compile_definitions(cflogger PRIVATE CFLOG_BUILDING)
endif()
