# CPU Module - compiled as OBJECT library for merging into cfbase.dll
cmake_minimum_required(VERSION 3.16)

add_library(cfbase_cpu STATIC)
set_target_properties(cfbase_cpu PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
# Define export macro since this will be linked into cfbase shared library
target_compile_definitions(cfbase_cpu PRIVATE CFBASE_EXPORTS)
# Foundation utils (aex) + base/include platform headers
target_link_libraries(cfbase_cpu PRIVATE cfbase_headers)
target_include_directories(cfbase_cpu
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)
target_sources(cfbase_cpu PRIVATE
    cfcpu.cpp
    cfcpu_profile.cpp
    cfcpu_bonus.cpp
)

if(WIN32)
    target_sources(cfbase_cpu PRIVATE
        private/win_impl/cpu_profile.cpp
        private/win_impl/cpu_bonus.cpp
        private/win_impl/cpu_features.cpp
        private/win_impl/cpu_info.cpp
    )
    target_include_directories(cfbase_cpu PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private
    )
elseif(UNIX)
    target_sources(cfbase_cpu PRIVATE
        private/linux_impl/cpu_profile.cpp
        private/linux_impl/cpu_bonus.cpp
        private/linux_impl/cpu_features.cpp
        private/linux_impl/cpu_info.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/linux/proc_parser.cpp
    )
endif()
