# Fundamental module — logging helpers and basic utilities
add_library(cffundamental STATIC)

target_sources(cffundamental PRIVATE
    src/log_helper.cpp
)

target_include_directories(cffundamental PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(cffundamental PUBLIC
    cflogger
    Qt6::Core
)

add_library(CFDesktop::fundamental ALIAS cffundamental)
