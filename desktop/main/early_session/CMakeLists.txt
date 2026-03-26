# Making Components with early sessions

add_library(CFDesktopEarlySession STATIC)

# Collect impl sources from subdirectory
add_subdirectory(impl)

target_sources(CFDesktopEarlySession PRIVATE
    init_chain/init_chain.cpp
    settings/early_settings.cpp
    early_handle/early_handle.cpp
    ${IMPL_SOURCES})

target_include_directories(CFDesktopEarlySession PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Link against cfbase to access device headers and other base utilities
# Link against cflogger to access cflog.h
# Link against CFDesktopLog for log_helper functions
target_link_libraries(CFDesktopEarlySession PUBLIC
    cfbase cflogger cfasciiart cffilesystem CFDesktopLog
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets)






