# GPU Module - compiled as STATIC library for merging into cfbase.dll
cmake_minimum_required(VERSION 3.16)

add_library(cfbase_gpu STATIC)
set_target_properties(cfbase_gpu PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    AUTOMOC OFF
    AUTORCC OFF
    AUTOUIC OFF
)
# Define export macro since this will be linked into cfbase shared library
target_compile_definitions(cfbase_gpu PRIVATE CFBASE_EXPORTS)
# Foundation utils (aex) + base/include platform headers
target_link_libraries(cfbase_gpu PRIVATE cfbase_headers)
target_include_directories(cfbase_gpu
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)
target_sources(cfbase_gpu PRIVATE
    gpu.cpp
)

if(WIN32)
    target_sources(cfbase_gpu PRIVATE
        private/win_impl/gpu_info.cpp
    )
    target_link_libraries(cfbase_gpu PRIVATE dxgi Shcore)
    target_include_directories(cfbase_gpu PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private
    )
elseif(UNIX)
    target_sources(cfbase_gpu PRIVATE
        private/linux_impl/gpu_info.cpp
    )
    # Optional: link libdrm if available
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(LIBDRM libdrm)
        if(LIBDRM_FOUND)
            target_link_libraries(cfbase_gpu PRIVATE ${LIBDRM_LIBRARIES})
            target_include_directories(cfbase_gpu PRIVATE ${LIBDRM_INCLUDE_DIRS})
            target_compile_definitions(cfbase_gpu PRIVATE HAS_LIBDRM=1)
        endif()
    endif()
endif()
