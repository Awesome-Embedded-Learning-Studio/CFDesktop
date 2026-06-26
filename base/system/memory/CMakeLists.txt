add_library(cfbase_memory STATIC)
set_target_properties(cfbase_memory PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
# Define export macro since this will be linked into cfbase shared library
target_compile_definitions(cfbase_memory PRIVATE CFBASE_EXPORTS)
# Foundation utils (aex) + base/include platform headers
target_link_libraries(cfbase_memory PRIVATE cfbase_headers)
target_include_directories(cfbase_memory
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)
target_sources(cfbase_memory PRIVATE
    memory_info.cpp
)

if(WIN32)
    target_sources(cfbase_memory PRIVATE
        private/win_impl/memory_info.cpp
        private/win_impl/physical_memory.cpp
        private/win_impl/swap_memory.cpp
        private/win_impl/process_memory.cpp
        private/win_impl/dimm_info.cpp
    )
elseif(UNIX)
    target_sources(cfbase_memory PRIVATE
        private/linux_impl/memory_info.cpp
        private/linux_impl/physical_memory.cpp
        private/linux_impl/swap_memory.cpp
        private/linux_impl/process_memory.cpp
        private/linux_impl/cached_memory.cpp
        private/linux_impl/dimm_info.cpp
    )
endif()
