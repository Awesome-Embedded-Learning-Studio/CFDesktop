add_library(cfbase_network STATIC
    network.cpp
)

target_compile_definitions(cfbase_network PRIVATE CFBASE_EXPORTS)

# Foundation utils (aex) + base/include platform headers
target_link_libraries(cfbase_network PRIVATE cfbase_headers)

target_link_libraries(cfbase_network
    PUBLIC  Qt6::Core
    PRIVATE Qt6::Network
)

target_include_directories(cfbase_network
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)