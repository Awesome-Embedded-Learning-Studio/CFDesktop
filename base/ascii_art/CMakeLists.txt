# ASCII Art module
# Static library
add_library(cfasciiart STATIC)

# Sources
target_sources(cfasciiart PRIVATE
    src/cfasciiart.cpp
)

# Include directories
target_include_directories(cfasciiart PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_include_directories(cfasciiart PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link dependencies
target_link_libraries(cfasciiart PUBLIC
    CFDesktop::base
)

# Alias for consistent naming
add_library(CFDesktop::ascii_art ALIAS cfasciiart)
