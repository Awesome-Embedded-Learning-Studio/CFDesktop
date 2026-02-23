# Memory Module Library
cmake_minimum_required(VERSION 3.16)

add_library(cfbase_memory STATIC)
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
    target_link_libraries(cfbase_memory PUBLIC psapi)
elseif(UNIX)
    target_sources(cfbase_memory PRIVATE
        private/linux_impl/memory_info.cpp
    )
endif()

# Link Qt Core for QtGlobal header
find_package(Qt6 COMPONENTS Core REQUIRED)
target_link_libraries(cfbase_memory PRIVATE Qt6::Core)

add_library(CFDesktop::base_memory ALIAS cfbase_memory)
