# CrashHandler module — async-signal-safe crash capture (Phase 1).
#
# STATIC lib; deliberately NOT linked to Qt: the signal handler path must stay
# free of Qt/global-object machinery (async-signal-safety). Uses only libc
# (backtrace/sigaction on Linux; SetUnhandledExceptionFilter arrives in a later
# phase on Windows). No Q_OBJECT, so AUTOMOC is not involved.
add_library(cfcrash STATIC)

target_sources(cfcrash
PRIVATE
    src/crash_handler.cpp
    src/crash_report.cpp
    src/crash_signal_handler.cpp
)

target_include_directories(cfcrash PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_library(CFDesktop::crash ALIAS cfcrash)
