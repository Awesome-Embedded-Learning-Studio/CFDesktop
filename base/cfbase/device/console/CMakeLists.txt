# Console Module - compiled as STATIC library for linking into cfbase.dll
cmake_minimum_required(VERSION 3.16)

add_library(cfbase_console STATIC)
set_target_properties(cfbase_console PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
# Define export macro since this will be linked into cfbase shared library
target_compile_definitions(cfbase_console PRIVATE CFBASE_EXPORTS)
target_include_directories(cfbase_console
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/impl
)
target_sources(cfbase_console PRIVATE
    impl/console_platform.cpp
    console_helper.cpp
)

if(WIN32)
    target_sources(cfbase_console PRIVATE
        impl/win/console_win_impl.cpp
    )
elseif(UNIX)
    target_sources(cfbase_console PRIVATE
        impl/unix/console_unix_impl.cpp
    )
endif()

target_link_libraries(cfbase_console PRIVATE cfbase_headers)
