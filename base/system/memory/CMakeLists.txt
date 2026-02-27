# Memory Module - compiled as OBJECT library for merging into cfbase.dll
cmake_minimum_required(VERSION 3.16)

add_library(cfbase_memory OBJECT)
set_target_properties(cfbase_memory PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
target_include_directories(cfbase_memory
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
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
