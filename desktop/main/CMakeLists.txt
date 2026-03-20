log_info("Desktop Self" "Start Configuring Desktop Main")

# Add log and path subdirectories first
add_subdirectory(log)
add_subdirectory(path)

add_executable(CFDesktop)

target_sources(CFDesktop PRIVATE
    main.cpp
    init/init_early_session.cpp)

# Add include directories for this executable
target_include_directories(CFDesktop PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Link against the unified CFDesktop shared library
# Also link CFDesktopEarlySession directly for symbols not exported from DLL
target_link_libraries(CFDesktop PRIVATE
    CFDesktop_shared CFDesktopEarlySession CFDesktopLog cflogger
)

log_info("Desktop Self" "Configured Desktop Main Done!")
