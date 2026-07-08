# IPC module — single-instance signaling + future IPC framework
# STATIC lib; line-based QLocalSocket protocol (QtCore-only, suits 6ULL rule:
# no D-Bus). Used by the single-instance lock to let a second shell instance
# ask the running one to raise itself.
add_library(cfipc STATIC)

target_sources(cfipc
PRIVATE
    src/ipc_server.cpp
    src/ipc_client.cpp
    # Listed explicitly so AUTOMOC scans the Q_OBJECT in ipc_server.h
    # (headers reached only via include dirs are not auto-discovered).
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cfipc/ipc_server.h
)

target_include_directories(cfipc PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(cfipc
PUBLIC
    Qt6::Core
    Qt6::Network
)

add_library(CFDesktop::ipc ALIAS cfipc)
